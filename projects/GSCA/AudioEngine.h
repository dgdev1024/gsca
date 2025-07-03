/**
 * @file    GSCA/AudioEngine.h
 */

#pragma once
#include <GSCA/Common.h>

/* Typedefs and Forward Declarations ******************************************/

typedef struct gscaAPU              gscaAPU;
typedef struct gscaAudioStore       gscaAudioStore;
typedef struct gscaAudioHandle      gscaAudioHandle;
typedef struct gscaAudioEngine      gscaAudioEngine;

/* Enumerations ***************************************************************/

typedef enum
{
    GSCA_NT_C = 1,
    GSCA_NT_CSHARP,
    GSCA_NT_D,
    GSCA_NT_DSHARP,
    GSCA_NT_E,
    GSCA_NT_F,
    GSCA_NT_FSHARP,
    GSCA_NT_G,
    GSCA_NT_GSHARP,
    GSCA_NT_A,
    GSCA_NT_ASHARP,
    GSCA_NT_B,
    GSCA_NT_COUNT = GSCA_NT_B
} gscaNote;

typedef enum
{
    GSCA_VC_CHAN1 = 0,
    GSCA_VC_CHAN2,
    GSCA_VC_CHAN3,
    GSCA_VC_CHAN4,
    GSCA_VC_CHAN5,
    GSCA_VC_CHAN6,
    GSCA_VC_CHAN7,
    GSCA_VC_CHAN8,

    GSCA_VC_MUSIC1 = GSCA_VC_CHAN1,
    GSCA_VC_MUSIC2 = GSCA_VC_CHAN2,
    GSCA_VC_MUSIC3 = GSCA_VC_CHAN3,
    GSCA_VC_MUSIC4 = GSCA_VC_CHAN4,
    GSCA_VC_SFX1 = GSCA_VC_CHAN5,
    GSCA_VC_SFX2 = GSCA_VC_CHAN6,
    GSCA_VC_SFX3 = GSCA_VC_CHAN7,
    GSCA_VC_SFX4 = GSCA_VC_CHAN8,

    GSCA_VC_MUSIC_COUNT = 4,
    GSCA_VC_SFX_COUNT = 4,
    GSCA_VC_COUNT = 8
} gscaVirtualChannel;

/* Unions *********************************************************************/

typedef union
{
    struct
    {
        uint8_t counter : 4;
        uint8_t pitch   : 1;
        uint8_t         : 2;
        uint8_t on      : 1;
    };
    uint8_t value;
} gscaLowHealthAlarm;

typedef union
{
    struct
    {
        uint8_t frameCount  : 6;
        uint8_t             : 1;
        uint8_t fadeIn      : 1;
    };
    uint8_t value;
} gscaMusicFade;

/* Public Functions ***********************************************************/

GSCA_API gscaAudioEngine* gscaCreateAudioEngine (gscaAPU* apu, gscaAudioStore*);
GSCA_API void gscaDestroyAudioEngine (gscaAudioEngine* engine);
GSCA_API void gscaInitAudioEngine (gscaAudioEngine* engine);
GSCA_API void gscaUpdateAudioEngine (gscaAudioEngine* engine);
GSCA_API int32_t gscaIsPlayingSFX (const gscaAudioEngine* engine);
GSCA_API bool gscaFadeToMusic (gscaAudioEngine* engine, const char* name, uint8_t length);
GSCA_API bool gscaPlayMusic (gscaAudioEngine* engine, const char* name);
GSCA_API bool gscaPlaySFX (gscaAudioEngine* engine, const char* name);
GSCA_API bool gscaPlayStereoSFX (gscaAudioEngine* engine, const char* name);
GSCA_API bool gscaPlayCry (gscaAudioEngine* engine, const char* name, int16_t pitch, int16_t length);