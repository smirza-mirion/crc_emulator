/*
 * platform_macos.c - macOS Platform Implementation
 *
 * Provides timer thread and platform utilities for macOS (ARM and x86).
 * Uses pthreads for the timer thread and dispatch timers or nanosleep
 * for precise timing.
 */
#ifdef __APPLE__

#include "platform.h"
#include "crc.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

/* Firmware interrupt handlers (implemented in hal_pit.c) */
extern void timed_interrupt(void);
extern void sec_interrupt(void);

/* Amulet bridge polling */
extern void amulet_bridge_poll_rx(void);
extern void ws_server_poll(void);

/* Timer thread state */
static pthread_t timer_thread;
static volatile int timer_running = 0;
static unsigned long start_time_ms = 0;

/* Get monotonic time in milliseconds */
unsigned long platform_get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long)(tv.tv_sec * 1000UL + tv.tv_usec / 1000UL);
}

/*
 * Timer thread function.
 * Runs at approximately 1ms intervals, calling timed_interrupt()
 * and sec_interrupt() at the appropriate times.
 */
static void *timer_thread_func(void *arg)
{
    (void)arg;
    unsigned long last_ms = platform_get_time_ms();
    unsigned long last_sec = last_ms;
    unsigned long last_ws_poll = last_ms;
    struct timespec ts;
    unsigned long dbg_timer_calls = 0;

    printf("[timer_thread] Started at %lu ms\n", last_ms);

    while (timer_running) {
        /* Sleep ~1ms */
        ts.tv_sec = 0;
        ts.tv_nsec = 1000000; /* 1ms */
        nanosleep(&ts, NULL);

        unsigned long now = platform_get_time_ms();
        unsigned long elapsed = now - last_ms;

        if (dbg_timer_calls < 5) {
            printf("[timer_thread] iter=%lu elapsed=%lu now=%lu\n",
                   dbg_timer_calls, elapsed, now);
        }

        /* Call timed_interrupt for each elapsed millisecond */
        unsigned long i;
        for (i = 0; i < elapsed && i < 10; i++) {
            timed_interrupt();
            dbg_timer_calls++;
        }
        last_ms = now;

        if (dbg_timer_calls == 100 || dbg_timer_calls == 500 || dbg_timer_calls == 1000) {
            extern volatile unsigned long int g_csec_tstamp;
            printf("[timer_thread] %lu ticks at %lu ms, g_csec=%lu\n",
                   dbg_timer_calls, now, g_csec_tstamp);
        }

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

    return NULL;
}

void platform_timer_start(void)
{
    if (timer_running) return;

    start_time_ms = platform_get_time_ms();
    timer_running = 1;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&timer_thread, &attr, timer_thread_func, NULL);
    pthread_attr_destroy(&attr);
}

void platform_timer_stop(void)
{
    if (!timer_running) return;

    timer_running = 0;
    pthread_join(timer_thread, NULL);
}

void platform_sleep_ms(int ms)
{
    if (ms <= 0) return;
    usleep((useconds_t)(ms * 1000));
}

void platform_sleep_us(int us)
{
    if (us <= 0) return;
    usleep((useconds_t)us);
}

void platform_ensure_data_dir(void)
{
    struct stat st;
    if (stat("data", &st) == -1) {
        mkdir("data", 0755);
    }
    /* Create subdirectories that the firmware expects */
    if (stat("data/data", &st) == -1) {
        mkdir("data/data", 0755);
    }
}

void platform_yield(void)
{
    /* Sleep 1ms to prevent 100% CPU in the main polling loop.
     * On real hardware, the loop is paced by the 1ms PIT interrupt;
     * in the emulator we need an explicit yield. */
    usleep(1000);
}

#endif /* __APPLE__ */
