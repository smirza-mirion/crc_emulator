/*
 * compat.h - Compatibility header for compiling CRC-25R firmware on desktop
 *
 * This header provides replacements for Green Hills Compiler (GHS) specific
 * constructs and Motorola ColdFire MCF5282 hardware-specific code so the
 * firmware can compile with GCC/Clang on macOS and Windows.
 */
#ifndef EMULATOR_COMPAT_H
#define EMULATOR_COMPAT_H

/* ---- POSIX Name Conflict Resolution ---- */
/* Handled via #ifdef EMULATOR guards in firmware headers (keyboard.h etc.) */

/* ---- GHS Compiler Intrinsics ---- */

/* __interrupt is already handled by crc.h:38-40 via __GNUC__ guard */

/* GHS intrinsics for interrupt enable/disable */
#ifndef __DI
#define __DI()  do { /* disable interrupts - no-op in emulator */ } while(0)
#endif

#ifndef __EI
#define __EI()  do { /* enable interrupts - no-op in emulator */ } while(0)
#endif

/* GHS assembly intrinsics */
#ifndef asm
/* GCC/Clang support asm() but ColdFire instructions (nop, halt) are not valid
   on x86/ARM. Replace with no-ops. */
#ifdef __GNUC__
/* Override only ColdFire-specific asm calls via the firmware code.
   We handle this by defining asm as a no-op macro when EMULATOR is set. */
#undef asm
#define asm(x)  do { /* ColdFire asm: x - no-op in emulator */ } while(0)
#endif
#endif

/* ---- GHS Runtime Functions ---- */

/* GHS provides these; standard C libraries may need explicit declarations */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* ---- ColdFire Volatile Types (non-volatile for emulator) ---- */
/* The firmware uses volatile types for memory-mapped I/O.
   In the emulator, the cf struct is regular RAM, but we keep
   the types to maintain source compatibility. */

/* ---- Memory-Mapped Hardware ---- */
/* The firmware accesses hardware at MCF5282_BASE (0x40000000).
   In the emulator, cf is just a global struct in normal RAM.
   No memory mapping needed. */

/* ---- printf redirect ---- */
/* The firmware's pf() function may need to be available */
#ifndef pf
#define pf printf
#endif

/* ---- Platform Detection ---- */
#if defined(__APPLE__) && defined(__MACH__)
#define PLATFORM_MACOS 1
#elif defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define PLATFORM_LINUX 1
#endif

/* ---- Thread Safety ---- */
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#define EMU_MUTEX          CRITICAL_SECTION
#define EMU_MUTEX_INIT(m)  InitializeCriticalSection(&(m))
#define EMU_MUTEX_LOCK(m)  EnterCriticalSection(&(m))
#define EMU_MUTEX_UNLOCK(m) LeaveCriticalSection(&(m))
#define EMU_MUTEX_DESTROY(m) DeleteCriticalSection(&(m))
#else
#include <pthread.h>
#define EMU_MUTEX          pthread_mutex_t
#define EMU_MUTEX_INIT(m)  pthread_mutex_init(&(m), NULL)
#define EMU_MUTEX_LOCK(m)  pthread_mutex_lock(&(m))
#define EMU_MUTEX_UNLOCK(m) pthread_mutex_unlock(&(m))
#define EMU_MUTEX_DESTROY(m) pthread_mutex_destroy(&(m))
#endif

/* ---- Byte Order ---- */
/* ColdFire is big-endian; desktop is little-endian.
   For most firmware operations this doesn't matter since we're
   not doing raw memory overlays. But if needed: */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define EMU_BIG_ENDIAN 1
#else
#define EMU_LITTLE_ENDIAN 1
#endif

/* ---- Flash Mirror ---- */
/* The firmware declares: extern vudword flash_mirror[0x80000];
   We provide a RAM-based version. */

/* ---- Vector Table ---- */
/* The firmware uses vector_base.table[] for exception handlers.
   In the emulator, this is unused but must exist for linking. */

#endif /* EMULATOR_COMPAT_H */
