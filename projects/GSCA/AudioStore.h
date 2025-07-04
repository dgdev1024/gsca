/**
 * @file    GSCA/AudioStore.h
 */

#pragma once
#include <GSCA/Common.h>

/* Typedefs and Forward Declarations ******************************************/

typedef struct gscaAudioStore   gscaAudioStore;

/* Audio Handle Structure *****************************************************/

typedef struct gscaAudioHandle
{
    char        name[GSCA_AS_HANDLE_NAME_STRLEN];
    uint64_t    offset;
    uint16_t    id;
} gscaAudioHandle;

/* Public Function Prototypes *************************************************/

GSCA_API gscaAudioStore* gscaCreateAudioStore (size_t initialCapacity);
GSCA_API void gscaDestroyAudioStore (gscaAudioStore* audioStore);
GSCA_API bool gscaReadAudioBuffer (gscaAudioStore* audioStore, const uint8_t* data, size_t size);
GSCA_API bool gscaReadAudioFile (gscaAudioStore* audioStore, const char* filename);
GSCA_API bool gscaWriteAudioFile (const gscaAudioStore* audioStore, const char* filename);
GSCA_API const gscaAudioHandle* gscaGetHandleByIndex (const gscaAudioStore* audioStore, size_t index);
GSCA_API const gscaAudioHandle* gscaGetHandleByID (const gscaAudioStore* audioStore, uint16_t id);
GSCA_API const gscaAudioHandle* gscaGetHandleByName (const gscaAudioStore* audioStore, const char* name);
GSCA_API const gscaAudioHandle* gscaAddAudio (gscaAudioStore* audioStore, const char* name, const uint8_t* data, uint32_t size);
GSCA_API const uint8_t* gscaGetAudioData (const gscaAudioStore* audioStore);
GSCA_API const size_t gscaGetAudioDataSize (const gscaAudioStore* audioStore);
GSCA_API const size_t gscaGetAudioCount (const gscaAudioStore* audioStore);
