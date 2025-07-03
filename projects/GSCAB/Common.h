/**
 * @file    GSCAB/Common.h
 */

#pragma once
#include <physfs.h>
#include <GSCA/Common.h>
#include <GSCA/Commands.h>
#include <GSCA/AudioEngine.h>

/* Function Macros ************************************************************/

#define gscabPhysfsErr()        PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())

/* Constant Macros ************************************************************/

#define GSCAB_NPOS              (size_t) -1
#define GSCAB_LEXEME_LEN        64
#define GSCAB_LABEL_LEN         144
#define GSCAB_DEFAULT_CAPACITY  8
