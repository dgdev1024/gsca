/**
 * @file    GSCAB/Token.h
 */

#pragma once
#include <GSCAB/Common.h>

/* Enumerations ***************************************************************/

typedef enum
{
    GSCAB_TT_UNKNOWN = 0,
    GSCAB_TT_PERIOD,
    GSCAB_TT_COLON,
    GSCAB_TT_COMMA,
    GSCAB_TT_INTEGER,
    GSCAB_TT_HEXADECIMAL,
    GSCAB_TT_IDENTIFIER,
    GSCAB_TT_BOOLEAN,
    GSCAB_TT_NOTE,
    GSCAB_TT_DATA,
    GSCAB_TT_COMMAND,
} gscabTokenType;

typedef enum
{
    GSCAB_DT_BYTE,
    GSCAB_DT_WORD,
    GSCAB_DT_DOUBLE_WORD,
    GSCAB_DT_QUAD_WORD
} gscabDataType;

/* Token Structure ************************************************************/

typedef struct
{
    char            lexeme[GSCAB_LEXEME_LEN];
    gscabTokenType  type;
    gscaEnum        param;
    uint8_t         size;
} gscabToken;

/* Public Function Prototypes *************************************************/

uint64_t            gscabGetInteger (const gscabToken* type);
int64_t             gscabGetSignedInteger (const gscabToken* type);
const gscabToken*   gscabLookupKeyword (const char* lexeme);
