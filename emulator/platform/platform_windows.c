/*
 * platform_windows.c - Windows Platform Implementation
 *
 * Provides timer thread and platform utilities for Windows (x86 and ARM).
 * Uses Windows threads and multimedia timers for precise timing.
 */
#ifdef _WIN32

#include "platform.h"
#include "crc.h"
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>

/* Firmware interrupt handlers (implemented in hal_pit.c) */
extern void timed_interrupt(void);
extern void sec_interrupt(void);

/* Amulet bridge polling */
extern void amulet_bridge_poll_rx(void);
extern void ws_server_poll(void);

/* Timer thread state */
static HANDLE timer_thread_handle = NULL;
static volatile int timer_running = 0;

/* Get monotonic time in milliseconds */
unsigned long platform_get_time_ms(void)
{
    return (unsigned long)GetTickCount();
}

/*
 * Timer thread function.
 * Runs at approximately 1ms intervals.
 */
static DWORD WINAPI timer_thread_func(LPVOID arg)
{
    (void)arg;
    unsigned long last_ms = platform_get_time_ms();
    unsigned long last_sec = last_ms;
    unsigned long last_ws_poll = last_ms;

    /* Request 1ms timer resolution */
    timeBeginPeriod(1);

    while (timer_running) {
        Sleep(1); /* ~1ms */

        unsigned long now = platform_get_time_ms();
        unsigned long elapsed = now - last_ms;

        /* Call timed_interrupt for each elapsed millisecond */
        unsigned long i;
        for (i = 0; i < elapsed && i < 10; i++) {
            timed_interrupt();
        }
        last_ms = now;

        /* Call sec_interrupt every second */
        if (now - last_sec >= 1000) {
            sec_interrupt();
            last_sec = now;
        }

        /* Poll WebSocket server every 10ms */
        if (now - last_ws_poll >= 10) {
            ws_server_poll();
            amulet_bridge_poll_rx();
            last_ws_poll = now;
        }
    }

    timeEndPeriod(1);
    return 0;
}

void platform_timer_start(void)
{
    if (timer_running) return;

    timer_running = 1;
    timer_thread_handle = CreateThread(
        NULL, 0, timer_thread_func, NULL, 0, NULL);
}

void platform_timer_stop(void)
{
    if (!timer_running) return;

    timer_running = 0;
    if (timer_thread_handle) {
        WaitForSingleObject(timer_thread_handle, 5000);
        CloseHandle(timer_thread_handle);
        timer_thread_handle = NULL;
    }
}

void platform_sleep_ms(int ms)
{
    if (ms <= 0) return;
    Sleep((DWORD)ms);
}

void platform_sleep_us(int us)
{
    if (us <= 0) return;
    /* Windows doesn't have usleep; use Sleep with minimum 1ms */
    if (us >= 1000) {
        Sleep((DWORD)(us / 1000));
    } else {
        /* Busy-wait for sub-millisecond precision */
        LARGE_INTEGER freq, start, now;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&start);
        double target = (double)us / 1000000.0;
        do {
            QueryPerformanceCounter(&now);
        } while ((double)(now.QuadPart - start.QuadPart) / freq.QuadPart < target);
    }
}

void platform_ensure_data_dir(void)
{
    struct stat st;
    if (stat("data", &st) == -1) {
        _mkdir("data");
    }
    if (stat("data\\data", &st) == -1) {
        _mkdir("data\\data");
    }
}

void platform_yield(void)
{
    Sleep(1);
}

#endif /* _WIN32 */
