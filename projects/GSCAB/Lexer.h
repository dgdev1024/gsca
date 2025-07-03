/**
 * @file    GSCAB/Lexer.h
 */

#pragma once
#include <GSCAB/Token.h>

/* Public Function Prototypes *************************************************/

void                gscabInitLexer ();
void                gscabShutdownLexer();
void                gscabClearSource ();
void                gscabResetPointer ();
bool                gscabLexFile (const char* path);
bool                gscabLexFolder (const char* path);
const gscabToken*   gscabNextToken ();
const gscabToken*   gscabExpectToken (gscabTokenType type);
const gscabToken*   gscabTokenAt (size_t offset);
