/**
 * @file    GSCAB/Lexer.c
 */

#include <GSCAB/Lexer.h>

/* Lexer Context **************************************************************/

static struct
{
    gscabToken*     tokens;
    size_t          tokenSize;
    size_t          tokenCapacity;
    size_t          tokenPointer;

    char*           code;
    size_t          index;
    size_t          line;
} lexer = {
    .tokens         = nullptr,
    .tokenSize      = 0,
    .tokenCapacity  = 0,
    .tokenPointer   = 0,
    .code           = nullptr,
    .index          = 0,
    .line           = 0
};

/* Private Function Prototypes ************************************************/

static void gscabResizeTokens ();
static bool gscabPushToken (gscabTokenType, const char*);
static bool gscabPushKeyword (const gscabToken*);
static bool gscabTokenizeIdentifier (char*);
static bool gscabTokenizeInteger (char*);
static bool gscabTokenizeHexadecimal (char*);
static bool gscabTokenizeSymbol (char*);
static bool gscabTokenize ();

/* Private Functions **********************************************************/

void gscabResizeTokens ()
{
    if (lexer.tokenSize + 1 >= lexer.tokenCapacity)
    {
        size_t tokenCapacity = lexer.tokenCapacity * 1.5f;
        gscabToken* tokens = gscaResize(lexer.tokens, tokenCapacity, gscabToken);
        gscaExpectp(tokens, "Could not resize tokens array");

        lexer.tokens        = tokens;
        lexer.tokenCapacity = tokenCapacity;
    }
}

bool gscabPushToken (gscabTokenType type, const char* lexeme)
{
    gscabResizeTokens();
    gscabToken* token = &lexer.tokens[lexer.tokenSize++];
    token->type = type;
    token->size = 0;

    if (lexeme != nullptr && lexeme[0] != '\0')
    {
        strncpy(token->lexeme, lexeme, GSCAB_LEXEME_LEN);
    }
    else
    {
        token->lexeme[0] = '\0';
    }

    return true;
}

bool gscabPushKeyword (const gscabToken* keyword)
{
    gscabResizeTokens();
    gscabToken* token = &lexer.tokens[lexer.tokenSize++];
    strncpy(token->lexeme, keyword->lexeme, GSCAB_LEXEME_LEN);
    token->type = keyword->type;
    token->param = keyword->param;
    token->size = keyword->size;

    return true;
}

bool gscabTokenizeIdentifier (char* c)
{
    char buffer[GSCAB_LEXEME_LEN] = { 0 };
    char lower[GSCAB_LEXEME_LEN] = { 0 };
    size_t length = 0;

    while (
        isalnum(*c) ||
        *c == '_' ||
        *c == '#' ||
        *c == '.'
    )
    {
        if (length >= GSCAB_LEXEME_LEN)
        {
            gscaErr("Identifier token is too long.\n");
            return false;
        }

        buffer[length]      = *c;
        lower[length++]     = tolower(*c);

        *c = lexer.code[lexer.index++];
    }

    *c = lexer.code[--lexer.index];
    buffer[length]  = '\0';
    lower[length]   = '\0';

    const gscabToken* keyword = gscabLookupKeyword(lower);
    if (keyword != nullptr)
    {
        return gscabPushKeyword(keyword);
    }
    else
    {
        return gscabPushToken(GSCAB_TT_IDENTIFIER, buffer);
    }
}

bool gscabTokenizeInteger (char* c)
{
    char buffer[GSCAB_LEXEME_LEN] = { 0 };
    size_t length = 0;

    if (*c == '-')
    {
        buffer[length++] = *c;
        *c = lexer.code[lexer.index++];
    }

    while (isdigit(*c))
    {
        if (length >= GSCAB_LEXEME_LEN)
        {
            gscaErr("Integer token is too long.\n");
            return false;
        }

        buffer[length++] = *c;
        *c = lexer.code[lexer.index++];
    }

    *c = lexer.code[--lexer.index];
    buffer[length]  = '\0';

    return gscabPushToken(GSCAB_TT_INTEGER, buffer);
}

bool gscabTokenizeHexadecimal (char* c)
{
    char buffer[GSCAB_LEXEME_LEN] = { 0 };
    size_t length = 0;

    *c = lexer.code[lexer.index++];
    while (isxdigit(*c))
    {
        if (length >= GSCAB_LEXEME_LEN)
        {
            gscaErr("Hexadecimal token is too long.\n");
            return false;
        }

        buffer[length++] = *c;
        *c = lexer.code[lexer.index++];
    }

    *c = lexer.code[--lexer.index];
    buffer[length]  = '\0';

    return gscabPushToken(GSCAB_TT_HEXADECIMAL, buffer);
}

bool gscabTokenizeSymbol (char* c)
{
    switch (*c)
    {
        case '.':   return gscabPushToken(GSCAB_TT_PERIOD, ".");
        case ':':   return gscabPushToken(GSCAB_TT_COLON, ":");
        case ',':   return gscabPushToken(GSCAB_TT_COMMA, ",");
        default:
            gscaErr("Unexpected character '%c'.\n", *c);
            return false;
    }
}

bool gscabTokenize ()
{
    bool ok = true;
    bool comment = false;

    while (true)
    {
        char c = lexer.code[lexer.index++];
        if (c == '\0')          { return true; }
        if (c == '\n')          { lexer.line++; comment = false; continue; }
        if (isspace(c))         { continue; }
        if (comment == true)    { continue; }
        if (c == ';')           { comment = true; continue; }

        if (isalpha(c) || c == '_' || c == '.') { ok = gscabTokenizeIdentifier(&c); }
        else if (isdigit(c) || c == '-')        { ok = gscabTokenizeInteger(&c); }
        else if (c == '$')                      { ok = gscabTokenizeHexadecimal(&c); }
        else                                    { ok = gscabTokenizeSymbol(&c); }

        if (ok == false)    { return false; }
    }
}

/* Public Functions ***********************************************************/

void gscabInitLexer ()
{
    lexer.tokens = gscaCreate(GSCAB_DEFAULT_CAPACITY, gscabToken);
    lexer.tokenCapacity = GSCAB_DEFAULT_CAPACITY;
    gscaExpectp(lexer.tokens, "Could not allocate tokens array");
}

void gscabShutdownLexer()
{
    gscabClearSource();
    gscaDestroy(lexer.tokens);
}

void gscabClearSource ()
{
    gscaDestroy(lexer.code);
}

void gscabResetPointer ()
{
    lexer.tokenPointer = 0;
}

bool gscabLexFile (const char* path)
{
    gscaExpect(path, "Pointer 'path' is NULL!\n");

    if (
        strcmp(path, "drumkits.asm") == 0 ||
        strcmp(path, "wave_samples.asm") == 0
    )
    {
        return true;
    }

    PHYSFS_File* file = PHYSFS_openRead(path);
    if (file == nullptr)
    {
        gscaErr("Could not open source file '%s' for reading: '%s'.\n",
            path, gscabPhysfsErr());
        return false;
    }

    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    if (size < 0)
    {
        gscaErr("Could not determine size of source file '%s': '%s'.\n",
            path, gscabPhysfsErr());
        PHYSFS_close(file);
        return false;
    }

    char* code = gscaResize(lexer.code, size + 1, char);
    if (code == nullptr)
    {
        gscaErrp("Could not (re-)allocate source code string to hold source file '%s'\n",
            path);
        PHYSFS_close(file);
        return false;
    }

    if (!PHYSFS_readBytes(file, code, size))
    {
        gscaErr("Could not read contents of source file '%s': '%s'.\n",
            path, gscabPhysfsErr());
        gscaDestroy(code);
        PHYSFS_close(file);
        return false;
    }

    PHYSFS_close(file);

    code[size] = '\0';
    lexer.code = code;
    lexer.index = 0;
    lexer.line = 1;

    if (gscabTokenize() == false)
    {
        gscaErr("Error tokenizing source file '%s' at line #%zu.\n", path,
            lexer.line);
        return false;
    }

    return true;
}

bool gscabLexFolder (const char* path)
{
    char** sources = PHYSFS_enumerateFiles(path);
    if (sources == nullptr)
    {
        gscaErr("Could not enumerate files in folder '%s': '%s'.\n", path,
            gscabPhysfsErr());
        return false;
    }
    
    bool ok = true;
    for (char** i = sources; *i != NULL; ++i)
    {
        PHYSFS_Stat stat;
        if (!PHYSFS_stat(*i, &stat))
        {
            gscaErr("Could not stat file '%s': '%s'.\n", path,
                gscabPhysfsErr());
            ok = false;
            break;
        }

        if (stat.filetype == PHYSFS_FILETYPE_REGULAR)
        {
            ok = gscabLexFile(*i);
            if (ok == false) { break; }
        }
    }

    PHYSFS_freeList(sources);
    return ok;
}

const gscabToken* gscabNextToken ()
{
    return (lexer.tokenPointer < lexer.tokenSize) ?
        (&lexer.tokens[lexer.tokenPointer++]) : nullptr;
}

const gscabToken* gscabExpectToken (gscabTokenType type)
{   
    if (
        lexer.tokenPointer < lexer.tokenSize &&
        lexer.tokens[lexer.tokenPointer].type == type
    )
    {
        return &lexer.tokens[lexer.tokenPointer++];
    }

    return nullptr;
}

const gscabToken* gscabTokenAt (size_t offset)
{
    return (lexer.tokenPointer + offset < lexer.tokenSize) ?
        (&lexer.tokens[lexer.tokenPointer + offset]) : nullptr;
}
