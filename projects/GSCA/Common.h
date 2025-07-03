/**
 * @file    GSCA/Common.h
 * @brief   Commonly-used include files, macros and typedefs.
 */

#pragma once

/* Include Files **************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

/* Object/Constant Macros *****************************************************/

#define GSCA_API
#define GSCA_MAJOR_VERSION              0x01
#define GSCA_MINOR_VERSION              0x00
#define GSCA_DEFAULT_SAMPLE_RATE        44100
#define GSCA_WAVE_RAM_SIZE              16
#define GSCA_WAVE_RAM_NIBBLE_SIZE       32
#define GSCA_UPDATE_INTERVAL            70224
#define GSCA_AS_HANDLE_NAME_STRLEN      64
#define GSCA_AS_DEFAULT_CAPACITY        0x400
#define GSCA_MAX_SIDE_VOLUME            0x7
#define GSCA_MAX_VOLUME                 0x77

/* Function Macros ************************************************************/

#define gscaLog(stream, ...)            fprintf(stream, "[%s] ", __FUNCTION__); fprintf(stream, __VA_ARGS__)
#define gscaOut(...)                    gscaLog(stdout, __VA_ARGS__)
#define gscaErr(...)                    gscaLog(stderr, __VA_ARGS__)
#define gscaErrp(...)                   { int e = errno; gscaErr(__VA_ARGS__); fprintf(stderr, ": '%s'\n", strerror(e)); }
#define gscaDie(...)                    gscaErr(__VA_ARGS__); exit(EXIT_FAILURE)
#define gscaDiep(...)                   gscaErrp(__VA_ARGS__); exit(EXIT_FAILURE)
#define gscaCheck(clause, ...)          if (!(clause)) { gscaErr(__VA_ARGS__); return; }
#define gscaCheckv(clause, val, ...)    if (!(clause)) { gscaErr(__VA_ARGS__); return val; }
#define gscaCheckp(clause, ...)         if (!(clause)) { gscaErrp(__VA_ARGS__); return; }
#define gscaCheckpv(clause, val, ...)   if (!(clause)) { gscaErrp(__VA_ARGS__); return val; }
#define gscaExpect(clause, ...)         if (!(clause)) { gscaDie(__VA_ARGS__); }
#define gscaExpectp(clause, ...)        if (!(clause)) { gscaDiep(__VA_ARGS__); }
#define gscaAssert(clause)              assert(clause)
#define gscaCreate(count, type)         ((type*) malloc(count * sizeof(type)))
#define gscaCreateZero(count, type)     ((type*) calloc(count, sizeof(type)))
#define gscaZero(ptr, count, type)      memset(ptr, 0x00, count * sizeof(type))
#define gscaResize(ptr, count, type)    ((type*) realloc(ptr, count * sizeof(type)))
#define gscaCopy(to, from, count, type) memcpy(to, from, count * sizeof(type))
#define gscaDestroy(ptr)                if (ptr != NULL) { free(ptr); } ptr = NULL;
#define gscaCopyString(to, from, len)   strncpy(to, from, len - 1); to[len - 1] = '\0'
#define gscaCheckBit(val, bit)          ((val >> bit) & 1)
#define gscaSetBit(val, bit)            val = (val | (1 << bit))
#define gscaClearBit(val, bit)          val = (val & ~(1 << bit))
#define gscaToggleBit(val, bit)         val = (val ^ (1 << bit))
#define gscaChangeBit(val, bit, on)     if (on) { gscaSetBit(val, bit); } else { gscaClearBit(val, bit); }

#define gscaCopyOffset(to, toOffset, from, fromOffset, count, type) \
    memcpy( \
        to + (toOffset * sizeof(type)), \
        from + (fromOffset * sizeof(type)), \
        count * sizeof(type) \
    )

/* Typedefs *******************************************************************/

typedef int gscaEnum;
