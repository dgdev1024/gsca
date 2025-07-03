/**
 * @file    GSCAB/Builder.h
 */

#pragma once
#include <GSCAB/Common.h>

/* Public Function Prototypes *************************************************/

void gscabInitBuilder ();
void gscabShutdownBuilder ();
bool gscabBuilderPassOne ();
bool gscabBuilderPassTwo ();
bool gscabSaveBuilderOutput (const char* filename);
