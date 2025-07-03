/**
 * @file    GSCAB/Main.c
 */

#include <GSCAB/Lexer.h>
#include <GSCAB/Builder.h>

/* Private Functions **********************************************************/

static void gscabInitPHYSFS (const char* arg0, const char* mountFolder)
{
    if (!PHYSFS_init(arg0))
    {
        gscaDie("Could not initialize PHYSFS: '%s'!\n", gscabPhysfsErr());
    }
    else if (!PHYSFS_mount(mountFolder, nullptr, 1))
    {
        gscaDie("Could not mount provided folder '%s': '%s'!\n", mountFolder,
            gscabPhysfsErr());
    }
}

static void gscabShutdownPHYSFS ()
{
    if (PHYSFS_isInit())
    {
        PHYSFS_deinit();
    }
}

static void gscabAtExit ()
{
    gscabShutdownBuilder();
    gscabShutdownLexer();
    gscabShutdownPHYSFS();
}

/* Public Functions ***********************************************************/

int main (int argc, char** argv)
{
    if (argc < 3)
    {
        printf("Usage: %s <folder> <output>\n", argv[0]);
        return 0;
    }

    atexit(gscabAtExit);
    gscabInitPHYSFS(argv[0], argv[1]);
    gscabInitLexer();
    gscabInitBuilder();

    if (gscabLexFolder("/") == false)
    {
        fprintf(stderr, "Error lexing folder '%s'.\n", argv[1]);
        return 1;
    }
    else
    {
        gscabClearSource();
    }

    if (gscabBuilderPassOne() == false)
    {
        fprintf(stderr, "Error running builder first pass.\n");
        return 2;
    }

    if (gscabBuilderPassTwo() == false)
    {
        fprintf(stderr, "Error running builder second pass.\n");
        return 3;
    }

    if (gscabSaveBuilderOutput(argv[2]) == false)
    {
        fprintf(stderr, "Error saving builder output to file '%s'.\n", argv[2]);
        return 4;
    }

    return 0;
}
