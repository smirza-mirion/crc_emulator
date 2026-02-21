/*
 * ws_server.c - Embedded WebSocket Server for CRC-25R Emulator
 *
 * Provides a WebSocket server that bridges the Amulet protocol bridge to
 * the web-based frontend. Handles:
 *   - TCP socket setup (cross-platform: BSD sockets / Winsock2)
 *   - WebSocket handshake (HTTP Upgrade with Sec-WebSocket-Accept)
 *   - WebSocket text frame send/receive (RFC 6455)
 *   - Broadcasting JSON messages to all connected clients
 *   - Serving static files from frontend/dist/ for the HTML frontend
 *
 * Maximum 4 concurrent WebSocket clients.
 *
 * The WebSocket handshake requires SHA-1 + Base64. A minimal SHA-1
 * implementation is included (just for computing the accept key).
 */

/* ---- Platform-specific socket headers ---- */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define SOCKET_INVALID INVALID_SOCKET
#define CLOSESOCKET(s) closesocket(s)
#define SOCKET_ERRNO WSAGetLastError()
#define SOCKET_WOULDBLOCK WSAEWOULDBLOCK
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
typedef int socket_t;
#define SOCKET_INVALID (-1)
#define CLOSESOCKET(s) close(s)
#define SOCKET_ERRNO errno
#define SOCKET_WOULDBLOCK EWOULDBLOCK
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- Constants ---- */
#define WS_MAX_CLIENTS   4
#define WS_RECV_BUF_SIZE 4096
#define WS_SEND_BUF_SIZE 4096
#define WS_GUID          "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

/* WebSocket opcodes */
#define WS_OP_CONTINUATION 0x00
#define WS_OP_TEXT         0x01
#define WS_OP_BINARY       0x02
#define WS_OP_CLOSE        0x08
#define WS_OP_PING         0x09
#define WS_OP_PONG         0x0A

/* ---- Client state ---- */
typedef enum {
    CLIENT_EMPTY = 0,        /* Slot is available */
    CLIENT_HTTP_HANDSHAKE,   /* Waiting for HTTP upgrade request */
    CLIENT_CONNECTED         /* WebSocket connection established */
} ClientState;

typedef struct {
    socket_t     sock;
    ClientState  state;
    char         recv_buf[WS_RECV_BUF_SIZE];
    int          recv_len;
} WsClient;

/* ---- Callback type ---- */
typedef void (*ws_message_callback)(const char *message, int len);

/* ---- Module state ---- */
static socket_t       listen_sock = SOCKET_INVALID;
static WsClient       clients[WS_MAX_CLIENTS];
static ws_message_callback msg_callback = NULL;
static int            server_running = 0;

/* Forward declaration for amulet_bridge full state push */
extern void amulet_bridge_send_full_state(void);

/* ---- Callback registration ---- */
void ws_set_message_callback(ws_message_callback cb)
{
    msg_callback = cb;
}

/* ==================================================================
 * Minimal SHA-1 implementation (RFC 3174)
 * Used only for the WebSocket handshake accept key computation.
 * ================================================================== */

typedef struct {
    unsigned int state[5];
    unsigned int count[2];
    unsigned char buffer[64];
} SHA1_CTX;

#define SHA1_ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

static void sha1_transform(unsigned int state[5], const unsigned char block[64])
{
    unsigned int a, b, c, d, e, w[80];
    int i;

    /* Expand 16 32-bit words into 80 */
    for (i = 0; i < 16; i++) {
        w[i] = ((unsigned int)block[i * 4]     << 24) |
               ((unsigned int)block[i * 4 + 1] << 16) |
               ((unsigned int)block[i * 4 + 2] << 8)  |
               ((unsigned int)block[i * 4 + 3]);
    }
    for (i = 16; i < 80; i++) {
        w[i] = SHA1_ROTL(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }

    a = state[0]; b = state[1]; c = state[2]; d = state[3]; e = state[4];

    for (i = 0; i < 80; i++) {
        unsigned int f, k, temp;
        if (i < 20) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        temp = SHA1_ROTL(a, 5) + f + e + k + w[i];
        e = d; d = c; c = SHA1_ROTL(b, 30); b = a; a = temp;
    }

    state[0] += a; state[1] += b; state[2] += c;
    state[3] += d; state[4] += e;
}

static void sha1_init(SHA1_CTX *ctx)
{
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count[0] = ctx->count[1] = 0;
}

static void sha1_update(SHA1_CTX *ctx, const unsigned char *data, unsigned int len)
{
    unsigned int i, j;

    j = (ctx->count[0] >> 3) & 63;
    if ((ctx->count[0] += (len << 3)) < (len << 3))
        ctx->count[1]++;
    ctx->count[1] += (len >> 29);

    if (j + len >= 64) {
        i = 64 - j;
        memcpy(&ctx->buffer[j], data, i);
        sha1_transform(ctx->state, ctx->buffer);
        for (; i + 63 < len; i += 64) {
            sha1_transform(ctx->state, &data[i]);
        }
        j = 0;
    } else {
        i = 0;
    }
    memcpy(&ctx->buffer[j], &data[i], len - i);
}

static void sha1_final(unsigned char digest[20], SHA1_CTX *ctx)
{
    unsigned char finalcount[8];
    unsigned char c;
    int i;

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((ctx->count[(i >= 4) ? 0 : 1]
                         >> ((3 - (i & 3)) * 8)) & 255);
    }
    c = 0x80;
    sha1_update(ctx, &c, 1);
    while ((ctx->count[0] & 504) != 448) {
        c = 0x00;
        sha1_update(ctx, &c, 1);
    }
    sha1_update(ctx, finalcount, 8);
    for (i = 0; i < 20; i++) {
        digest[i] = (unsigned char)((ctx->state[i >> 2]
                     >> ((3 - (i & 3)) * 8)) & 255);
    }
}

/* ==================================================================
 * Base64 encoding (for WebSocket handshake)
 * ================================================================== */

static const char b64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int base64_encode(const unsigned char *in, int in_len, char *out, int out_size)
{
    int i, j;
    unsigned int v;

    j = 0;
    for (i = 0; i < in_len; i += 3) {
        v = (unsigned int)in[i] << 16;
        if (i + 1 < in_len) v |= (unsigned int)in[i + 1] << 8;
        if (i + 2 < in_len) v |= (unsigned int)in[i + 2];

        if (j + 4 >= out_size) break;
        out[j++] = b64_table[(v >> 18) & 0x3F];
        out[j++] = b64_table[(v >> 12) & 0x3F];
        out[j++] = (i + 1 < in_len) ? b64_table[(v >> 6) & 0x3F] : '=';
        out[j++] = (i + 2 < in_len) ? b64_table[v & 0x3F] : '=';
    }
    out[j] = '\0';
    return j;
}

/* ==================================================================
 * Socket Helpers
 * ================================================================== */

/*
 * Set a socket to non-blocking mode.
 */
static void set_nonblocking(socket_t sock)
{
#ifdef _WIN32
    unsigned long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    }
#endif
}

/*
 * Set TCP_NODELAY to reduce latency.
 */
static void set_nodelay(socket_t sock)
{
    int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag));
}

/* ==================================================================
 * WebSocket Handshake
 * ================================================================== */

/*
 * Extract the value of a specific HTTP header from a request string.
 * Returns pointer to value start or NULL if not found.
 */
static const char *find_header(const char *request, const char *header_name)
{
    const char *p = request;
    int hlen = (int)strlen(header_name);

    while ((p = strstr(p, header_name)) != NULL) {
        /* Check that this is at the start of a line (preceded by \r\n or start) */
        if (p == request || (p >= request + 2 && *(p - 1) == '\n' && *(p - 2) == '\r')) {
            p += hlen;
            /* Skip ': ' */
            if (*p == ':') {
                p++;
                while (*p == ' ') p++;
                return p;
            }
        }
        p++;
    }
    return NULL;
}

/*
 * Extract a header value into a buffer (up to \r\n or end of string).
 */
static int extract_header_value(const char *request, const char *header_name,
                                char *out, int out_size)
{
    const char *val = find_header(request, header_name);
    int i = 0;
    if (!val) return 0;

    while (*val && *val != '\r' && *val != '\n' && i < out_size - 1) {
        out[i++] = *val++;
    }
    out[i] = '\0';
    return (i > 0) ? 1 : 0;
}

/*
 * Compute Sec-WebSocket-Accept value from the client's Sec-WebSocket-Key.
 * accept_out must be at least 29 bytes.
 */
static void compute_accept_key(const char *client_key, char *accept_out, int accept_size)
{
    char combined[128];
    unsigned char sha1_digest[20];
    SHA1_CTX sha1;

    snprintf(combined, sizeof(combined), "%s%s", client_key, WS_GUID);

    sha1_init(&sha1);
    sha1_update(&sha1, (const unsigned char *)combined, (unsigned int)strlen(combined));
    sha1_final(sha1_digest, &sha1);

    base64_encode(sha1_digest, 20, accept_out, accept_size);
}

/*
 * Try to complete the WebSocket handshake for a client.
 * Returns 1 if handshake completed, 0 if need more data, -1 on error.
 */
static int try_handshake(WsClient *client)
{
    char ws_key[64];
    char accept_key[64];
    char response[512];
    int response_len;

    /* Check if we have a complete HTTP request (ends with \r\n\r\n) */
    if (!strstr(client->recv_buf, "\r\n\r\n")) {
        return 0;  /* Need more data */
    }

    /* Check for WebSocket upgrade request */
    if (!strstr(client->recv_buf, "Upgrade: websocket") &&
        !strstr(client->recv_buf, "Upgrade: WebSocket")) {
        /* Not a WebSocket request - could be a static file request.
         * For now, send a simple response and close. */
        const char *http_ok =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><h1>CRC-25R Emulator WebSocket Server</h1>"
            "<p>Connect via WebSocket at ws://localhost:PORT/</p>"
            "</body></html>";
        send(client->sock, http_ok, (int)strlen(http_ok), 0);
        return -1;  /* Close connection */
    }

    /* Extract Sec-WebSocket-Key */
    if (!extract_header_value(client->recv_buf, "Sec-WebSocket-Key", ws_key, sizeof(ws_key))) {
        return -1;  /* Bad request */
    }

    /* Compute accept key */
    compute_accept_key(ws_key, accept_key, sizeof(accept_key));

    /* Send handshake response */
    response_len = snprintf(response, sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n",
        accept_key);

    send(client->sock, response, response_len, 0);

    /* Clear receive buffer */
    client->recv_len = 0;
    memset(client->recv_buf, 0, sizeof(client->recv_buf));

    printf("[ws_server] WebSocket handshake complete\n");
    return 1;
}

/* ==================================================================
 * WebSocket Frame Handling
 * ================================================================== */

/*
 * Send a WebSocket text frame to a specific client.
 * Returns number of bytes sent or -1 on error.
 */
static int ws_send_frame(socket_t sock, const char *data, int data_len)
{
    unsigned char header[10];
    int header_len = 0;
    int total_sent = 0;
    int ret;

    /* First byte: FIN bit + text opcode */
    header[0] = 0x80 | WS_OP_TEXT;
    header_len = 1;

    /* Payload length encoding (server frames are NOT masked) */
    if (data_len <= 125) {
        header[1] = (unsigned char)data_len;
        header_len = 2;
    } else if (data_len <= 65535) {
        header[1] = 126;
        header[2] = (unsigned char)((data_len >> 8) & 0xFF);
        header[3] = (unsigned char)(data_len & 0xFF);
        header_len = 4;
    } else {
        /* 64-bit length - not expected for our JSON messages */
        header[1] = 127;
        memset(&header[2], 0, 4);  /* High 4 bytes = 0 */
        header[6] = (unsigned char)((data_len >> 24) & 0xFF);
        header[7] = (unsigned char)((data_len >> 16) & 0xFF);
        header[8] = (unsigned char)((data_len >> 8) & 0xFF);
        header[9] = (unsigned char)(data_len & 0xFF);
        header_len = 10;
    }

    /* Send header */
    ret = send(sock, (const char *)header, header_len, 0);
    if (ret <= 0) return -1;
    total_sent += ret;

    /* Send payload */
    if (data_len > 0) {
        ret = send(sock, data, data_len, 0);
        if (ret <= 0) return -1;
        total_sent += ret;
    }

    return total_sent;
}

/*
 * Send a WebSocket close frame to a client.
 */
static void ws_send_close(socket_t sock)
{
    unsigned char frame[2];
    frame[0] = 0x80 | WS_OP_CLOSE;
    frame[1] = 0;
    send(sock, (const char *)frame, 2, 0);
}

/*
 * Send a WebSocket pong frame (in response to a ping).
 */
static void ws_send_pong(socket_t sock, const unsigned char *payload, int payload_len)
{
    unsigned char header[2];
    header[0] = 0x80 | WS_OP_PONG;
    header[1] = (unsigned char)(payload_len & 0x7F);
    send(sock, (const char *)header, 2, 0);
    if (payload_len > 0) {
        send(sock, (const char *)payload, payload_len, 0);
    }
}

/*
 * Try to parse a WebSocket frame from the client's receive buffer.
 * Returns the number of bytes consumed, or 0 if incomplete, or -1 on error.
 *
 * On success, payload is written to payload_out (must be at least recv_buf_size).
 * payload_len_out is set to the payload length.
 * opcode_out is set to the frame opcode.
 */
static int ws_parse_frame(const unsigned char *buf, int buf_len,
                          unsigned char *payload_out, int *payload_len_out,
                          int *opcode_out)
{
    int offset = 0;
    int payload_len;
    int mask_bit;
    unsigned char mask_key[4];
    int i;

    if (buf_len < 2) return 0;  /* Need at least 2 bytes for header */

    /* First byte: FIN + opcode */
    *opcode_out = buf[0] & 0x0F;
    /* int fin = (buf[0] >> 7) & 1; -- not needed for now */

    /* Second byte: MASK + payload length */
    mask_bit = (buf[1] >> 7) & 1;
    payload_len = buf[1] & 0x7F;
    offset = 2;

    if (payload_len == 126) {
        if (buf_len < 4) return 0;  /* Need more data */
        payload_len = ((int)buf[2] << 8) | (int)buf[3];
        offset = 4;
    } else if (payload_len == 127) {
        if (buf_len < 10) return 0;
        /* 64-bit length; we only support up to 32-bit for sanity */
        payload_len = ((int)buf[6] << 24) | ((int)buf[7] << 16)
                    | ((int)buf[8] << 8)  | (int)buf[9];
        offset = 10;
    }

    /* Mask key (4 bytes if mask bit is set; clients must always mask) */
    if (mask_bit) {
        if (buf_len < offset + 4) return 0;
        memcpy(mask_key, &buf[offset], 4);
        offset += 4;
    }

    /* Check if we have the full payload */
    if (buf_len < offset + payload_len) return 0;

    /* Extract and unmask payload */
    if (payload_len > WS_RECV_BUF_SIZE - 1) {
        /* Payload too large */
        return -1;
    }

    memcpy(payload_out, &buf[offset], payload_len);
    if (mask_bit) {
        for (i = 0; i < payload_len; i++) {
            payload_out[i] ^= mask_key[i % 4];
        }
    }
    payload_out[payload_len] = '\0';

    *payload_len_out = payload_len;
    return offset + payload_len;
}

/* ==================================================================
 * Client Management
 * ================================================================== */

/*
 * Disconnect and reset a client slot.
 */
static void disconnect_client(int index)
{
    if (clients[index].sock != SOCKET_INVALID) {
        CLOSESOCKET(clients[index].sock);
    }
    clients[index].sock = SOCKET_INVALID;
    clients[index].state = CLIENT_EMPTY;
    clients[index].recv_len = 0;
    memset(clients[index].recv_buf, 0, sizeof(clients[index].recv_buf));
}

/*
 * Process received data for a client in HTTP handshake state.
 */
static void process_handshake(int index)
{
    int result = try_handshake(&clients[index]);

    if (result == 1) {
        /* Handshake complete - transition to WebSocket mode */
        clients[index].state = CLIENT_CONNECTED;
        /* Send the current display state to the newly connected client */
        amulet_bridge_send_full_state();
    } else if (result == -1) {
        /* Error or non-WebSocket request */
        disconnect_client(index);
    }
    /* result == 0: need more data, keep waiting */
}

/*
 * Process received data for a connected WebSocket client.
 */
static void process_ws_data(int index)
{
    unsigned char payload[WS_RECV_BUF_SIZE];
    int payload_len = 0;
    int opcode = 0;
    int consumed;

    while (clients[index].recv_len > 0) {
        consumed = ws_parse_frame(
            (const unsigned char *)clients[index].recv_buf,
            clients[index].recv_len,
            payload, &payload_len, &opcode);

        if (consumed == 0) break;   /* Incomplete frame */
        if (consumed < 0) {
            disconnect_client(index);
            return;
        }

        /* Handle frame based on opcode */
        switch (opcode) {
            case WS_OP_TEXT:
                /* Deliver text message to the callback */
                if (msg_callback && payload_len > 0) {
                    msg_callback((const char *)payload, payload_len);
                }
                break;

            case WS_OP_CLOSE:
                ws_send_close(clients[index].sock);
                disconnect_client(index);
                return;

            case WS_OP_PING:
                ws_send_pong(clients[index].sock, payload, payload_len);
                break;

            case WS_OP_PONG:
                /* Ignore pong */
                break;

            default:
                break;
        }

        /* Remove consumed bytes from receive buffer */
        if (consumed < clients[index].recv_len) {
            memmove(clients[index].recv_buf,
                    clients[index].recv_buf + consumed,
                    clients[index].recv_len - consumed);
        }
        clients[index].recv_len -= consumed;
    }
}

/* ==================================================================
 * Public API
 * ================================================================== */

/*
 * Initialize the WebSocket server on the specified port.
 * Creates a non-blocking listening TCP socket.
 */
void ws_server_init(int port)
{
    struct sockaddr_in addr;
    int opt = 1;
    int i;

#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    /* Initialize client slots */
    for (i = 0; i < WS_MAX_CLIENTS; i++) {
        clients[i].sock = SOCKET_INVALID;
        clients[i].state = CLIENT_EMPTY;
        clients[i].recv_len = 0;
    }

    /* Create listening socket */
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == SOCKET_INVALID) {
        printf("[ws_server] ERROR: Failed to create socket\n");
        return;
    }

    /* Allow address reuse */
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&opt, sizeof(opt));

    /* Bind to port */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((unsigned short)port);

    if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("[ws_server] ERROR: Failed to bind to port %d\n", port);
        CLOSESOCKET(listen_sock);
        listen_sock = SOCKET_INVALID;
        return;
    }

    /* Listen with small backlog */
    if (listen(listen_sock, 4) < 0) {
        printf("[ws_server] ERROR: Failed to listen\n");
        CLOSESOCKET(listen_sock);
        listen_sock = SOCKET_INVALID;
        return;
    }

    /* Set non-blocking */
    set_nonblocking(listen_sock);

    server_running = 1;
    printf("[ws_server] Listening on port %d (max %d clients)\n",
           port, WS_MAX_CLIENTS);
}

/*
 * Non-blocking poll for new connections and incoming data.
 * Should be called periodically (e.g., from amulet_bridge_poll_rx).
 */
void ws_server_poll(void)
{
    struct timeval tv;
    fd_set read_fds;
    socket_t max_fd;
    int i, n;

    if (!server_running || listen_sock == SOCKET_INVALID) return;

    /* Build fd_set */
    FD_ZERO(&read_fds);
    FD_SET(listen_sock, &read_fds);
    max_fd = listen_sock;

    for (i = 0; i < WS_MAX_CLIENTS; i++) {
        if (clients[i].sock != SOCKET_INVALID) {
            FD_SET(clients[i].sock, &read_fds);
            if (clients[i].sock > max_fd) {
                max_fd = clients[i].sock;
            }
        }
    }

    /* Non-blocking select (timeout = 0) */
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    n = select((int)(max_fd + 1), &read_fds, NULL, NULL, &tv);
    if (n <= 0) return;

    /* Check for new connections */
    if (FD_ISSET(listen_sock, &read_fds)) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        socket_t new_sock = accept(listen_sock,
                                   (struct sockaddr *)&client_addr, &addr_len);

        if (new_sock != SOCKET_INVALID) {
            /* Find an empty client slot */
            int slot = -1;
            for (i = 0; i < WS_MAX_CLIENTS; i++) {
                if (clients[i].state == CLIENT_EMPTY) {
                    slot = i;
                    break;
                }
            }

            if (slot >= 0) {
                set_nonblocking(new_sock);
                set_nodelay(new_sock);
                clients[slot].sock = new_sock;
                clients[slot].state = CLIENT_HTTP_HANDSHAKE;
                clients[slot].recv_len = 0;
                printf("[ws_server] New connection (slot %d) from %s:%d\n",
                       slot,
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));
            } else {
                /* No free slots */
                printf("[ws_server] Connection rejected: max clients reached\n");
                CLOSESOCKET(new_sock);
            }
        }
    }

    /* Process data from connected clients */
    for (i = 0; i < WS_MAX_CLIENTS; i++) {
        if (clients[i].sock == SOCKET_INVALID) continue;
        if (!FD_ISSET(clients[i].sock, &read_fds)) continue;

        /* Read available data */
        {
            int space = WS_RECV_BUF_SIZE - clients[i].recv_len - 1;
            int bytes_read;

            if (space <= 0) {
                printf("[ws_server] Client %d recv buffer full, disconnecting\n", i);
                disconnect_client(i);
                continue;
            }

            bytes_read = recv(clients[i].sock,
                              clients[i].recv_buf + clients[i].recv_len,
                              space, 0);

            if (bytes_read <= 0) {
                /* Connection closed or error */
                if (bytes_read == 0) {
                    printf("[ws_server] Client %d disconnected\n", i);
                }
#ifndef _WIN32
                else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    printf("[ws_server] Client %d recv error: %d\n", i, errno);
                }
#endif
                if (bytes_read <= 0) {
                    disconnect_client(i);
                    continue;
                }
            }

            clients[i].recv_len += bytes_read;
            clients[i].recv_buf[clients[i].recv_len] = '\0';
        }

        /* Process based on state */
        switch (clients[i].state) {
            case CLIENT_HTTP_HANDSHAKE:
                process_handshake(i);
                break;
            case CLIENT_CONNECTED:
                process_ws_data(i);
                break;
            default:
                break;
        }
    }
}

/*
 * Broadcast a message to all connected WebSocket clients.
 */
void ws_broadcast(const char *message, int len)
{
    int i;

    if (!server_running) return;

    for (i = 0; i < WS_MAX_CLIENTS; i++) {
        if (clients[i].state == CLIENT_CONNECTED &&
            clients[i].sock != SOCKET_INVALID) {
            if (ws_send_frame(clients[i].sock, message, len) < 0) {
                printf("[ws_server] Failed to send to client %d, disconnecting\n", i);
                disconnect_client(i);
            }
        }
    }
}

/*
 * Shut down the WebSocket server and close all connections.
 */
void ws_server_shutdown(void)
{
    int i;

    server_running = 0;

    /* Close all client connections */
    for (i = 0; i < WS_MAX_CLIENTS; i++) {
        if (clients[i].state == CLIENT_CONNECTED) {
            ws_send_close(clients[i].sock);
        }
        disconnect_client(i);
    }

    /* Close listening socket */
    if (listen_sock != SOCKET_INVALID) {
        CLOSESOCKET(listen_sock);
        listen_sock = SOCKET_INVALID;
    }

#ifdef _WIN32
    WSACleanup();
#endif

    printf("[ws_server] Server shut down\n");
}
