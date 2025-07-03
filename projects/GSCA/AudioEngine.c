/**
 * @file    GSCA/AudioEngine.c
 */

#include <GSCA/Commands.h>
#include <GSCA/APU.h>
#include <GSCA/AudioStore.h>
#include <GSCA/AudioEngine.h>
#define ctx engine->context

/* Private Constants - Drum Instruments ***************************************/

static const uint8_t GSCA_DRUM00[] = {
    GSCA_NOISE_NOTE(32, 1, 1, 0),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE1[] = {
    GSCA_NOISE_NOTE(32, 12, 1, 51),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE2[] = {
    GSCA_NOISE_NOTE(32, 11, 1, 51),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE3[] = {
    GSCA_NOISE_NOTE(32, 10, 1, 51),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE4[] = {
    GSCA_NOISE_NOTE(32, 8, 1, 51),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM05[] = {
    GSCA_NOISE_NOTE(39, 8, 4, 55),
    GSCA_NOISE_NOTE(38, 8, 4, 54),
    GSCA_NOISE_NOTE(37, 8, 3, 53),
    GSCA_NOISE_NOTE(36, 8, 3, 52),
    GSCA_NOISE_NOTE(35, 8, 2, 51),
    GSCA_NOISE_NOTE(34, 8, 1, 50),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_TRIANGLE1[] = {
    GSCA_NOISE_NOTE(32, 5, 1, 42),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_TRIANGLE2[] = {
    GSCA_NOISE_NOTE(33, 4, 1, 43),
    GSCA_NOISE_NOTE(32, 6, 1, 42),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_HIHAT1[] = {
    GSCA_NOISE_NOTE(32, 8, 1, 16),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE5[] = {
    GSCA_NOISE_NOTE(32, 8, 2, 35),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE6[] = {
    GSCA_NOISE_NOTE(32, 8, 2, 37),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE7[] = {
    GSCA_NOISE_NOTE(32, 8, 2, 38),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_HIHAT2[] = {
    GSCA_NOISE_NOTE(32, 10, 1, 16),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_HIHAT3[] = {
    GSCA_NOISE_NOTE(32, 10, 2, 17),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE8[] = {
    GSCA_NOISE_NOTE(32, 10, 2, 80),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_TRIANGLE3[] = {
    GSCA_NOISE_NOTE(32, 10, 1, 24),
    GSCA_NOISE_NOTE(32, 3, 1, 51),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_TRIANGLE4[] = {
    GSCA_NOISE_NOTE(34, 9, 1, 40),
    GSCA_NOISE_NOTE(32, 7, 1, 24),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE9[] = {
    GSCA_NOISE_NOTE(32, 9, 1, 34),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE10[] = {
    GSCA_NOISE_NOTE(32, 7, 1, 34),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE11[] = {
    GSCA_NOISE_NOTE(32, 6, 1, 34),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM20[] = {
    GSCA_NOISE_NOTE(32, 1, 1, 17),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM21[] = {
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE12[] = {
    GSCA_NOISE_NOTE(32, 9, 1, 51),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE13[] = {
    GSCA_NOISE_NOTE(32, 5, 1, 50),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_SNARE14[] = {
    GSCA_NOISE_NOTE(32, 8, 1, 49),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_KICK1[] = {
    GSCA_NOISE_NOTE(32, 8, 8, 107),
    GSCA_NOISE_NOTE(32, 7, 1, 0),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_TRIANGLE5[] = {
    GSCA_NOISE_NOTE(48, 9, 1, 24),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM27[] = {
    GSCA_NOISE_NOTE(39, 9, 2, 16),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM28[] = {
    GSCA_NOISE_NOTE(51, 9, 1, 0),
    GSCA_NOISE_NOTE(51, 1, 1, 0),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM29[] = {
    GSCA_NOISE_NOTE(51, 9, 1, 17),
    GSCA_NOISE_NOTE(51, 1, 1, 0),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_CRASH1[] = {
    GSCA_NOISE_NOTE(51, 8, 8, 21),
    GSCA_NOISE_NOTE(32, 6, 5, 18),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM31[] = {
    GSCA_NOISE_NOTE(51, 5, 1, 33),
    GSCA_NOISE_NOTE(51, 1, 1, 17),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM32[] = {
    GSCA_NOISE_NOTE(51, 5, 1, 80),
    GSCA_NOISE_NOTE(51, 1, 1, 17),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM33[] = {
    GSCA_NOISE_NOTE(32, 10, 1, 49),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_CRASH2[] = {
    GSCA_NOISE_NOTE(32, 8, 4, 18),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM35[] = {
    GSCA_NOISE_NOTE(51, 8, 1, 0),
    GSCA_NOISE_NOTE(51, 1, 1, 0),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_DRUM36[] = {
    GSCA_NOISE_NOTE(51, 8, 1, 33),
    GSCA_NOISE_NOTE(51, 1, 1, 17),
    GSCA_SOUND_RET
};

static const uint8_t GSCA_KICK2[] = {
    GSCA_NOISE_NOTE(32, 10, 8, 107),
    GSCA_NOISE_NOTE(32, 7, 1, 0),
    GSCA_SOUND_RET
};

/* Private Constants - Drumkits ***********************************************/

static const uint8_t *GSCA_DRUMKIT0[] = {
    GSCA_DRUM00,
    GSCA_SNARE1,
    GSCA_SNARE2,
    GSCA_SNARE3,
    GSCA_SNARE4,
    GSCA_DRUM05,
    GSCA_TRIANGLE1,
    GSCA_TRIANGLE2,
    GSCA_HIHAT1,
    GSCA_SNARE5,
    GSCA_SNARE6,
    GSCA_SNARE7,
    GSCA_HIHAT2
};

static const uint8_t *GSCA_DRUMKIT1[] = {
    GSCA_DRUM00,
    GSCA_HIHAT1,
    GSCA_SNARE5,
    GSCA_SNARE6,
    GSCA_SNARE7,
    GSCA_HIHAT2,
    GSCA_HIHAT3,
    GSCA_SNARE8,
    GSCA_TRIANGLE3,
    GSCA_TRIANGLE4,
    GSCA_SNARE9,
    GSCA_SNARE10,
    GSCA_SNARE11
};

static const uint8_t *GSCA_DRUMKIT2[] = {
    GSCA_DRUM00,
    GSCA_SNARE1,
    GSCA_SNARE9,
    GSCA_SNARE10,
    GSCA_SNARE11,
    GSCA_DRUM05,
    GSCA_TRIANGLE1,
    GSCA_TRIANGLE2,
    GSCA_HIHAT1,
    GSCA_SNARE5,
    GSCA_SNARE6,
    GSCA_SNARE7,
    GSCA_HIHAT2
};

static const uint8_t *GSCA_DRUMKIT3[] = {
    GSCA_DRUM21,
    GSCA_SNARE12,
    GSCA_SNARE13,
    GSCA_SNARE14,
    GSCA_KICK1,
    GSCA_TRIANGLE5,
    GSCA_DRUM20,
    GSCA_DRUM27,
    GSCA_DRUM28,
    GSCA_DRUM29,
    GSCA_DRUM21,
    GSCA_KICK2,
    GSCA_CRASH2
};

static const uint8_t *GSCA_DRUMKIT4[] = {
    GSCA_DRUM21,
    GSCA_DRUM20,
    GSCA_SNARE13,
    GSCA_SNARE14,
    GSCA_KICK1,
    GSCA_DRUM33,
    GSCA_TRIANGLE5,
    GSCA_DRUM35,
    GSCA_DRUM31,
    GSCA_DRUM32,
    GSCA_DRUM36,
    GSCA_KICK2,
    GSCA_CRASH1
};

static const uint8_t *GSCA_DRUMKIT5[] = {
    GSCA_DRUM00,
    GSCA_SNARE9,
    GSCA_SNARE10,
    GSCA_SNARE11,
    GSCA_DRUM27,
    GSCA_DRUM28,
    GSCA_DRUM29,
    GSCA_DRUM05,
    GSCA_TRIANGLE1,
    GSCA_CRASH1,
    GSCA_SNARE14,
    GSCA_SNARE13,
    GSCA_KICK2
};

/* Private Constants **********************************************************/

static const uint8_t** GSCA_DRUMKIT_COLLECTION[] = {
    GSCA_DRUMKIT0,
    GSCA_DRUMKIT1,
    GSCA_DRUMKIT2,
    GSCA_DRUMKIT3,
    GSCA_DRUMKIT4,
    GSCA_DRUMKIT5
};

#define GSCA_WAVE_SAMPLE_COUNT 10

static const char GSCA_WAVE_STRINGS[GSCA_WAVE_SAMPLE_COUNT][33] = {
    "02468ACEFFFEDDCBBA98765444332211",
    "02468ACEEFFFFEEECCBBA98765432211",
    "1369BDEEEEFFFFEDDEFFFFEEEEDB9631",
    "02468ACDEFFEDEFFEEDCBA9876543210",
    "012345678ACDEEF77FEEDCA876543210",
    "0011223344332211FFEECCAA88AACCEE",
    "02468ACECBA98765FFFEEDDC44332211",
    "C0A987F5FFFEEDDC443322F102468ACE",
    "4433221F00468ACEF8FEEDDCCBA98765",
    "110000080013579AB4BAA99887654321"
};

static const uint16_t GSCA_FREQUENCY_TABLE[] = {
    0x0000,  // __
    0XF82C,  // C_
    0XF89D,  // C#
    0XF907,  // D_
    0XF96B,  // D#
    0XF9CA,  // E_
    0XFA23,  // F_
    0XFA77,  // F#
    0XFAC7,  // G_
    0XFB12,  // G#
    0XFB58,  // A_
    0XFB9B,  // A#
    0XFBDA,  // B_

    0XFC16,  // C_
    0XFC4E,  // C#
    0XFC83,  // D_
    0XFCB5,  // D#
    0XFCE5,  // E_
    0XFD11,  // F_
    0XFD3B,  // F#
    0XFD63,  // G_
    0XFD89,  // G#
    0XFDAC,  // A_
    0XFDCD,  // A#
    0XFDED,  // B_
};

/* Virtual Channel Structure **************************************************/

typedef struct
{
    union 
    {
        struct 
        {
            uint8_t channelOn           : 1;
            uint8_t subroutine          : 1;
            uint8_t looping             : 1;
            uint8_t sfx                 : 1;
            uint8_t noise               : 1;
            uint8_t cry                 : 1;
            uint8_t                     : 2;
            uint8_t vibrato             : 1;
            uint8_t pitchSlide          : 1;
            uint8_t dutyLoop            : 1;
            uint8_t                     : 1;
            uint8_t pitchOffsetEnabled  : 1;
            uint8_t                     : 1;
            uint8_t                     : 1;
            uint8_t                     : 1;
            uint8_t vibratoDir          : 1;
            uint8_t pitchSlideDir       : 1;
            uint8_t                     : 6;
        };
        uint8_t     channelFlags[3];
    };

    union 
    {
        struct 
        {
            uint8_t dutyOverride    : 1;
            uint8_t freqOverride    : 1;
            uint8_t                 : 1;
            uint8_t pitchSweep      : 1;
            uint8_t noiseSampling   : 1;
            uint8_t rest            : 1;
            uint8_t vibratoOverride : 1;
            uint8_t                 : 1;
        };
        uint8_t noteFlags;
    };

    uint16_t    musicId;
    uint64_t    musicAddress;
    uint64_t    lastMusicAddress;
    uint16_t    unused;
    uint8_t     condition;
    uint8_t     dutyCycle;
    uint8_t     volumeEnvelope;
    uint16_t    frequency;
    uint8_t     pitch;
    uint8_t     octave;
    uint8_t     transposition;
    uint8_t     noteDuration;
    uint16_t    noteDurationFraction;
    uint8_t     loopCount;
    uint16_t    tempo;
    uint8_t     tracks;
    uint8_t     dutyCyclePattern;
    uint8_t     vibratoDelayCount;
    uint8_t     vibratoDelay;
    uint8_t     vibratoExtent;
    uint8_t     vibratoRate;
    uint16_t    pitchSlideTarget;
    uint8_t     pitchSlideAmount;
    uint8_t     pitchSlideAmountFraction;
    uint16_t    field25;
    uint16_t    pitchOffset;
    uint8_t     field29;
    uint16_t    field2a;
    uint8_t     field2c;
    uint8_t     noteLength;
    uint8_t     field2e;
    uint8_t     field2f;
    uint16_t    field30;
    uint8_t     unknown0f;
} gscaChannelStruct;

/* Audio Engine Structure *****************************************************/

typedef struct gscaAudioEngine
{
    struct
    {
        gscaChannelStruct       channels[GSCA_VC_COUNT];
        uint8_t                 currentTrackDuty;
        uint8_t                 currentTrackEnvelope;
        uint16_t                currentTrackFrequency;
        uint8_t                 currentNoteDuration;
        uint8_t                 currentMusicByte;
        uint8_t                 currentChannelIndex;
        gscaMasterVolume        volume;
        gscaSoundPanning        soundOutput;
        gscaFrequencySweep      pitchSweep;
        uint16_t                musicId;
        const uint8_t*          noiseSampleAddress;
        uint8_t                 noiseSampleDelay;
        uint8_t                 musicNoiseSampleSet;
        uint8_t                 sfxNoiseSampleSet;
        gscaLowHealthAlarm      lowHealthAlarm;
        gscaMusicFade           musicFade;
        uint8_t                 musicFadeCount;
        uint16_t                musicFadeId;
        uint16_t                cryPitch;
        uint16_t                cryLength;
        uint8_t                 lastVolume;
        uint8_t                 sfxPriority;
        uint8_t                 channelJumpConditions[GSCA_VC_MUSIC_COUNT];
        uint8_t                 stereoPanningMask;
        uint8_t                 cryTracks;
        uint8_t                 sfxDuration;
        uint8_t                 currentSfx;
    } context;

    gscaAPU*                    apu;
    gscaAudioStore*             store;
    bool                        musicPlaying;
    bool                        dontPlayMapMusicOnReload;
    bool                        stereo;
    uint8_t                     mapMusic;
} gscaAudioEngine;

/* Private Function Prototypes ************************************************/

static gscaChannelStruct*   gscaCurrentChannel (gscaAudioEngine*);
static void                 gscaMusicFadeRestart (gscaAudioEngine*);
static void                 gscaMusicOn (gscaAudioEngine*);
static void                 gscaMusicOff (gscaAudioEngine*);
static void                 gscaUpdateChannel (gscaAudioEngine*);
static void                 gscaPlayDangerTone (gscaAudioEngine*);
static void                 gscaFadeMusic (gscaAudioEngine*);
static void                 gscaLoadNote (gscaAudioEngine*);
static void                 gscaHandleTrackVibrato (gscaAudioEngine*);
static void                 gscaApplyPitchSlide (gscaAudioEngine*);
static void                 gscaHandleNoise (gscaAudioEngine*);
static void                 gscaReadNoiseSample (gscaAudioEngine*);
static void                 gscaParseMusic (gscaAudioEngine*);
static void                 gscaRestoreVolume (gscaAudioEngine*);
static void                 gscaParseSoundEffect (gscaAudioEngine*);
static void                 gscaGetNoiseSample (gscaAudioEngine*);
static void                 gscaParseMusicCommand (gscaAudioEngine*);
static uint8_t              gscaGetMusicByte (gscaAudioEngine*);
static uint16_t             gscaGetFrequency (gscaAudioEngine*, uint8_t, uint8_t);
static void                 gscaSetNoteDuration (gscaAudioEngine*, uint8_t);
static void                 gscaSetGlobalTempo (gscaAudioEngine*, uint16_t);
static void                 gscaStartChannel (gscaAudioEngine*);
static void                 gscaSetLRTracks (gscaAudioEngine*, uint8_t);
static void                 gscaPlayStereoLoadedSFX (gscaAudioEngine*, uint16_t);
static uint64_t             gscaLoadChannel (gscaAudioEngine*, uint64_t);
static void                 gscaChannelInit (gscaAudioEngine*, uint8_t);
static uint8_t              gscaLoadMusicByte (gscaAudioEngine*, uint64_t);
static const uint8_t*       gscaGetLRTracks (gscaAudioEngine*);
static void                 gscaFadeToLoadedMusic (gscaAudioEngine*, uint16_t, uint8_t);
static void                 gscaPlayLoadedMusic (gscaAudioEngine*, uint16_t);
static void                 gscaPlayLoadedSFX (gscaAudioEngine*, uint16_t);
static void                 gscaPlayLoadedCry (gscaAudioEngine*, uint16_t);
static void                 gscaClearChannels (gscaAudioEngine*);
static void                 gscaClearChannel (gscaAudioEngine*, gscaAudioChannel);

/* Private Functions **********************************************************/

gscaChannelStruct* gscaCurrentChannel (gscaAudioEngine* engine)
{
    return &ctx.channels[ctx.currentChannelIndex & 0b111];
}

void gscaMusicFadeRestart (gscaAudioEngine* engine)
{
    uint16_t musicId = ctx.musicFadeId;
    gscaInitAudioEngine(engine);
    ctx.musicFadeId = musicId;
}

void gscaMusicOn (gscaAudioEngine* engine)
{
    engine->musicPlaying = true;
}

void gscaMusicOff (gscaAudioEngine* engine)
{
    engine->musicPlaying = false;
}

void gscaUpdateChannel (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);
    uint8_t nr52 = gscaReadNR52(engine->apu);
    uint8_t frequencyLow = (ctx.currentTrackFrequency & 0xFF);
    uint8_t frequencyHigh = ((ctx.currentTrackFrequency >> 8) & 0b111);

    switch (ctx.currentChannelIndex)
    {
        case GSCA_VC_CHAN1:
        {
            if (ctx.lowHealthAlarm.on == true)
                break;
        }
        case GSCA_VC_CHAN5:
        {
            uint8_t nr11 = gscaReadNR11(engine->apu);

            if (channel->pitchSweep == true)
            {
                gscaWriteNR10(engine->apu, ctx.pitchSweep.value);
            }

            if (channel->rest == true)
            {
                gscaWriteNR52(engine->apu, nr52 & 0b10001110);
                gscaClearChannel(engine, GSCA_AC_PULSE1);
            }
            else if (channel->noiseSampling == true)
            {
                gscaWriteNR11(engine->apu, (nr11 & 0x3F) | ctx.currentTrackDuty);
                gscaWriteNR12(engine->apu, ctx.currentTrackEnvelope);
                gscaWriteNR13(engine->apu, frequencyLow);
                gscaWriteNR14(engine->apu, frequencyHigh | 0x80);
            }
            else if (channel->freqOverride == true)
            {
                gscaWriteNR13(engine->apu, frequencyLow);
                gscaWriteNR14(engine->apu, frequencyHigh);

                if (channel->dutyOverride == true)
                {
                    gscaWriteNR11(engine->apu, (nr11 & 0x3F) | ctx.currentTrackDuty);
                }
            }
            else if (channel->vibratoOverride == true)
            {
                gscaWriteNR11(engine->apu, (nr11 & 0x3F) | ctx.currentTrackDuty);
                gscaWriteNR13(engine->apu, frequencyLow);
            }
            else if (channel->dutyOverride == true)
            {
                gscaWriteNR11(engine->apu, (nr11 & 0x3F) | ctx.currentTrackDuty);
            }
        } break;
        case GSCA_VC_CHAN2:
        case GSCA_VC_CHAN6:
        {
            uint8_t nr21 = gscaReadNR21(engine->apu);

            if (channel->rest == true)
            {
                gscaWriteNR52(engine->apu, nr52 & 0b10001101);
                gscaClearChannel(engine, GSCA_AC_PULSE2);
            }
            else if (channel->noiseSampling == true)
            {
                gscaWriteNR21(engine->apu, (nr21 & 0x3F) | ctx.currentTrackDuty);
                gscaWriteNR22(engine->apu, ctx.currentTrackEnvelope);
                gscaWriteNR23(engine->apu, frequencyLow);
                gscaWriteNR24(engine->apu, frequencyHigh | 0x80);
            }
            else if (channel->freqOverride == true)
            {
                gscaWriteNR23(engine->apu, frequencyLow);
                gscaWriteNR24(engine->apu, frequencyHigh);
            }
            else if (channel->vibratoOverride == true)
            {
                gscaWriteNR21(engine->apu, (nr21 & 0x3F) | ctx.currentTrackDuty);
                gscaWriteNR23(engine->apu, frequencyLow);
            }
            else if (channel->dutyOverride == true)
            {
                gscaWriteNR21(engine->apu, (nr21 & 0x3F) | ctx.currentTrackDuty);
            }
        } break;
        case GSCA_VC_CHAN3:
        case GSCA_VC_CHAN7:
        {
            if (channel->rest == true)
            {
                gscaWriteNR52(engine->apu, nr52 & 0b10001011);
                gscaClearChannel(engine, GSCA_AC_WAVE);
            }
            else if (channel->noiseSampling == true)
            {
                gscaWriteNR31(engine->apu, 0x3F);
                {
                    gscaWriteNR30(engine->apu, 0x00);
                    gscaSetWavePattern(
                        engine->apu, 
                        GSCA_WAVE_STRINGS[
                            (ctx.currentTrackEnvelope & 0xF) % 
                                GSCA_WAVE_SAMPLE_COUNT
                    ]);
                    gscaWriteNR32(engine->apu, (ctx.currentTrackEnvelope & 0x30) << 1);
                    gscaWriteNR30(engine->apu, 0x80);
                }
                gscaWriteNR33(engine->apu, frequencyLow);
                gscaWriteNR34(engine->apu, frequencyHigh | 0x80);
            }
            else if (channel->vibratoOverride == true)
            {
                gscaWriteNR33(engine->apu, frequencyLow);
            }
        } break;
        case GSCA_VC_CHAN4:
        case GSCA_VC_CHAN8:
        {
            if (channel->rest == true)
            {
                gscaWriteNR52(engine->apu, nr52 & 0b10000111);
                gscaClearChannel(engine, GSCA_AC_NOISE);
            }
            else if (channel->noiseSampling == true)
            {
                gscaWriteNR41(engine->apu, 0x3F);
                gscaWriteNR42(engine->apu, ctx.currentTrackEnvelope);
                gscaWriteNR43(engine->apu, frequencyLow);
                gscaWriteNR44(engine->apu, 0x80);
            }
        } break;
    }
}

void gscaPlayDangerTone (gscaAudioEngine* engine)
{
    // Only play the danger tone if the low health alarm is on, and if there
    // is no SFX currently playing.
    gscaLowHealthAlarm* alarm = &ctx.lowHealthAlarm;
    if (alarm->on == true && gscaIsPlayingSFX(engine) == 0)
    {
        if (alarm->counter == 0)
        {
            uint16_t pitch = (alarm->pitch == true) ? 0x750 : 0x6EE;
            gscaWriteNR10(engine->apu, 0x00);
            gscaWriteNR11(engine->apu, 0x80);
            gscaWriteNR12(engine->apu, 0xE2);
            gscaWriteNR13(engine->apu, pitch & 0xFF);
            gscaWriteNR14(engine->apu, (pitch >> 8) | 0x80);
        }

        if (++alarm->counter == 0)
        {
            alarm->pitch = !alarm->pitch;
        }

        alarm->on = 1;
        ctx.soundOutput.value |= 0x11;
    }
}

void gscaFadeMusic (gscaAudioEngine* engine)
{
    // Are we fading audio?
    if (ctx.musicFade.value != 0)
    {
        // If the fade count has not yet lapsed, decrement it.
        if (ctx.musicFadeCount > 0)
            { ctx.musicFadeCount--; }
        else
        {
            uint8_t currentVolume = ctx.volume.rightVolume;     // Get the side 1 (right) volume.
            ctx.musicFadeCount = ctx.musicFade.frameCount;      // Reset the fade count.

            // Check the fade direction. Are we fading in (1) or out (0)?
            if (ctx.musicFade.fadeIn == true)
            {
                // Once max volume has been reached, we will have finished fading in.
                // At this point, turn the music fade off.
                if (ctx.volume.rightVolume == GSCA_MAX_SIDE_VOLUME)
                {
                    ctx.musicFade.value = 0;
                    return;
                }

                currentVolume++;    // Increment the volume.
            }
            else
            {
                // Once zero volume is reached, we will have finished fading out.
                if (ctx.volume.rightVolume == 0)
                {
                    ctx.volume.value = 0;
                    gscaMusicFadeRestart(engine);               // Once fading has finished, restart the audio engine
                    gscaPlayLoadedMusic(engine, ctx.musicFadeId);   // and start playing the requested song.
                    ctx.musicFade.value = 0;
                    return;
                }

                currentVolume--;    // Decrement the volume.
            }

            // Update the current volume.
            ctx.volume.leftVolume   = currentVolume;
            ctx.volume.rightVolume  = currentVolume;
        }
    }
}

void gscaLoadNote (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);
    if (channel->pitchSlide == 1)
	{

		// Deduce the new note's duration from the note duration settings in the
		// current channel and in audio RAM. Ensure that it is not zero or negative.
		int16_t noteDuration = (channel->noteDuration - ctx.currentNoteDuration);
		if (noteDuration < 1) { noteDuration = 1; }

		// Record the new note duration.
		ctx.currentNoteDuration = noteDuration;

		// Determine the impact of the pitch side on the channel's frequency, based on
		// the pitch slide direction.
		uint16_t frequencyDifference = 0;
		if (channel->frequency > channel->pitchSlideTarget)
		{
			channel->pitchSlideDir = 0;
			frequencyDifference = channel->frequency - channel->pitchSlideTarget;
		}
		else
		{
			channel->pitchSlideDir = 1;
			frequencyDifference = channel->pitchSlideTarget - channel->frequency;
		}

		// Set the new pitch slide amount based on the frequency difference and note duration.
		channel->pitchSlideAmount				= (frequencyDifference / noteDuration);
		channel->pitchSlideAmountFraction	    = (frequencyDifference % noteDuration);
		channel->field25 						= 0;

	}
}

void gscaHandleTrackVibrato (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

	// Is the current track on a duty cycle loop? If so, then update the current 
	// duty cycle to the next iteration of that loop.
	if (channel->dutyLoop == 1)
	{
		channel->dutyCyclePattern = (
			(channel->dutyCyclePattern << 2) |
			(channel->dutyCyclePattern >> 6)
		);
		ctx.currentTrackDuty = (channel->dutyCyclePattern & 0b11000000);

		// Set the coresponding channel's duty override flag, too.
		channel->dutyOverride = 1;
	}

	// Does the current track have pitch offset enabled? If so, then adjust its
	// frequency to the coresponding channel's pitch offset setting.
	if (channel->pitchOffsetEnabled == 1)
	{
		ctx.currentTrackFrequency += channel->pitchOffset;
	}

	// Does the current track have vibrato enabled?
	if (channel->vibrato == 1)
	{
		// Check its delay count. If it's not zero, then just decrement it for
		// now.
		if (channel->vibratoDelayCount > 0)
		{
			channel->vibratoDelayCount--;
			return;
		}

		// Don't bother updating vibrato if there's no extent.
		if (channel->vibratoExtent == 0)
		{
			return;
		}

		// The current vibrato timer value is stored in the lower nibble of the
		// vibrato rate. Don't bother updating vibrato if this value is not zero.
		if ((channel->vibratoRate & 0xF) > 0)
		{
			channel->vibratoRate--;
			return;
		}

		// Use the upper nibble of the vibrato rate to refresh the timer.
		channel->vibratoRate |= (channel->vibratoRate >> 4);

		// Get the track's vibrato extent, and the bytes of its frequency.
		uint8_t vibratoExtent = channel->vibratoExtent;
		uint8_t frequencyLow = ctx.currentTrackFrequency & 0xFF;
		uint8_t frequencyHigh = (ctx.currentTrackFrequency & 0xFF00) >> 8;

		// Toggle the channel's vibrato direction up or down. Affect its vibrato
		// extent and frequency as approprite.
		if (channel->vibratoDir == 1)
		{

			// We are bending upwards.
			//
			// Reset the vibrato direction flag, so that the next direction is
			// downwards.
			channel->vibratoDir = 0;

			// Subtract the low byte of the vibrato extent from the low byte of
			// the frequency.
			int16_t difference = (frequencyLow - (vibratoExtent & 0xF));

			// Cap the frequency low at zero if the difference is below zero.
			if (difference < 0)
			{
				frequencyLow = 0;
			}
			else
			{
				frequencyLow = difference;
			}

		}
		else
		{

			// We are bending downwards.
			//
			// Set the vibrato direction flag so that the next direction is
			// upwards.
			channel->vibratoDir = 1;

			// Get the high byte of the vibrato extent. We'll add this value to
			// our frequency low.
			uint8_t extentHigh = (channel->vibratoExtent & 0xF0) >> 4;

			// Get the sum.
			uint16_t sum = frequencyLow + extentHigh;

			// Add the extent high to our frequency low, capping the sum at $FF
			// in case of overflow.
			if (sum > 0xFF)
			{
				frequencyLow = 0xFF;
			}
			else
			{
				frequencyLow = sum;
			}

		}

		// Update the current track's frequency and set the vibrato override.
		ctx.currentTrackFrequency = (frequencyHigh << 8) | frequencyLow;
		channel->vibratoOverride = 1;
	}
}

void gscaApplyPitchSlide (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

    // Only apply a pitch slide if it's active.
	if (channel->pitchSlide == 1)
	{
		// Keep track of the channel's frequency, and when we're done with the
		// pitch slide.
		uint16_t frequency = channel->frequency;
		bool done = false;

		// Check the pitch slide direction and apply as appropriate.
		if (channel->pitchSlideDir == 1)
		{
			// We are sliding upwards.
			frequency += channel->pitchSlideAmount;
			if (
				channel->field25 + channel->pitchSlideAmountFraction > 0xFF
			)
			{
				frequency++;
			}

			channel->field25 += channel->pitchSlideAmountFraction;
			done = (frequency > channel->pitchSlideTarget);
		}
		else
		{
			// We are sliding downwards.
			frequency -= channel->pitchSlideAmount;
			if (channel->field25 * 2 > 0xFF)
			{
				frequency--;
			}

			channel->field25 *= 2;
			done = (frequency < channel->pitchSlideTarget);
		}

		// Are we done sliding?
		if (done == true)
		{
			channel->pitchSlide = 0;
			channel->pitchSlideDir = 0;
		}
		else
		{
			channel->frequency = frequency;
			channel->dutyOverride = 1;
			channel->freqOverride = 1;
		}
	}
}

void gscaHandleNoise (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

	// Does the current channel have noise sampling enabled? If so, then check
	// for one of the following:
	// - Are we working on an SFX channel?
	// - Is `CHAN8` (the noise SFX channel) currently off?
	// - Is `CHAN8` on, but not currently playing noise?
	if (
		channel->noise == 1 &&
		(
			ctx.currentChannelIndex >= GSCA_VC_SFX1 ||
			ctx.channels[GSCA_VC_SFX4].channelOn == 0 ||
			ctx.channels[GSCA_VC_SFX4].noise == 0
		)
	)
	{

		// Is there a noise sample delay currently in progress?
		if (ctx.noiseSampleDelay == 0)
		{
			// If there isn't, then read the next noise sample.
			gscaReadNoiseSample(engine);
		}
		else
		{
			// Otherwise, decrement the delay and move along.
			ctx.noiseSampleDelay--;
		}

	}
}

void gscaReadNoiseSample (gscaAudioEngine* engine)
{
	// Don't read any noise sample if...
	// - ...the noise sample pointer is not pointing to anything.
	// - ...the noise sample pointer is pointing to a `SOUND_RET` command.
	if (
		ctx.noiseSampleAddress != NULL &&
		*ctx.noiseSampleAddress != GSCA_SOUND_RET_CMD
	)
	{
		// Adjust the current noise sample delay, volume envelope and frequency
		// according to the noise command bytes.
		ctx.noiseSampleDelay        = (*(ctx.noiseSampleAddress++) & 0xF) + 1;
		ctx.currentTrackEnvelope    = (*(ctx.noiseSampleAddress++));
		ctx.currentTrackFrequency   = (*(ctx.noiseSampleAddress++));

		// Enable noise sampling.
		gscaCurrentChannel(engine)->noiseSampling = 1;
	}
}

void gscaParseMusic (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

	// Keep track of the next music command.
	uint8_t musicCommand = 0x00;

	// Loop, parsing bytes until a note is read, or the current music song ends.
	while (true)
	{

		// Parse the next music command and place it here.
		musicCommand = gscaGetMusicByte(engine);

		// If the next command is a `SOUND_RET` command, and a music subroutine
		// is not currently being processed, then we can assume that the end of
		// the current song has been reached.
		if (
			musicCommand == GSCA_SOUND_RET_CMD &&
			channel->subroutine == 0
		)
		{

			// Check for one of the following:
			// - Are we working an SFX channel?
			// - Are we working a music channel, and is its coresponding SFX channel turned off?
			if (
				ctx.currentChannelIndex >= GSCA_VC_MUSIC_COUNT ||
				ctx.channels[
					ctx.currentChannelIndex + GSCA_VC_MUSIC_COUNT
				].channelOn == 0
			)	
			{

				// If so, then...
				// - ...if this channel is playing a cry, then restore volume.
				// - ...if this is an SFX channel on PC1, disable the sweep control.
				if (channel->cry == 1)
				{
					gscaRestoreVolume(engine);
				}

				if (ctx.currentChannelIndex == GSCA_VC_SFX1)
				{
					gscaWriteNR10(engine->apu, 0x00);
				}

			}

			// Disable this channel, set its rest note flag and clear its music ID.
			channel->channelOn = 0;
			channel->rest = 1;
			channel->musicId = 0;

			return;

		}

		// Is the next command a music note?
		else if (musicCommand < GSCA_FIRST_MUSIC_CMD)
		{
		
			// Is this channel processing an SFX or a cry?
			if (
				channel->sfx == 1 ||
				channel->cry == 1
			)
			{
				gscaParseSoundEffect(engine);
			}

			// Is this channel processing noise?
			else if (channel->noise == 1)
			{
				gscaGetNoiseSample(engine);
			}

			// Otherwise, we are processing a music note.
			else
			{

				// The lower nibble of the music command byte is the note's duration.
				gscaSetNoteDuration(engine, ctx.currentMusicByte & 0xF);

				// The upper nibble of the command byte is the note.
				uint8_t note = ctx.currentMusicByte >> 4;

				// A note value of zero indicates a rest. If this is encountered, then set
				// the channel's rest flag.
				if (note == 0)
				{
					channel->rest = 1;
				}
				else
				{
					// Update the channel's pitch and frequency values based on the note that
					// was retrieved. Enable the channel's noise sampling flag thereafter.
					channel->pitch = note;
					channel->frequency = gscaGetFrequency(engine, note, channel->octave);
					channel->noiseSampling = 1;

					// Load the next note.
					gscaLoadNote(engine);
				}

			}

			return;

		}

		// This has to be a music command. Parse it and continue in the loop.
		else
		{
			gscaParseMusicCommand(engine);
		}

	}
}

void gscaRestoreVolume (gscaAudioEngine* engine)
{
	// This function does nothing if we are not working SFX channel 1 (`CHAN5`).
	if (ctx.currentChannelIndex == GSCA_VC_SFX1)
	{

		// Reset the pitch offsets on SFX channels 2 and 4 (`CHAN6` and `CHAN8`).
		ctx.channels[GSCA_VC_SFX2].pitchOffset = 0;
		ctx.channels[GSCA_VC_SFX4].pitchOffset = 0;

		// Restore the volume.
		ctx.volume.value = ctx.lastVolume;

		// Reset the previous volume value and disable SFX priority.
		ctx.lastVolume = 0;
		ctx.sfxPriority = 0;

	}
}

void gscaParseSoundEffect (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

	// Enable noise sampling for this channel.
	channel->noiseSampling = 1;

	// Set the next note's duration according to the current music byte.
	gscaSetNoteDuration(engine, ctx.currentMusicByte);

	// The next music byte should contain the note's volume envelope setting.
	channel->volumeEnvelope = gscaGetMusicByte(engine);

	// The next music byte (or two music bytes for the non-noise channels) should
	// contain the next frequency.
	uint16_t frequency = gscaGetMusicByte(engine);
	if (
		ctx.currentChannelIndex != GSCA_VC_CHAN4 &&
		ctx.currentChannelIndex != GSCA_VC_CHAN8
	)
	{
		frequency |= (gscaGetMusicByte(engine) << 8);
	}

	// Update the channel's frequency.
	channel->frequency = frequency;
}

void gscaGetNoiseSample (gscaAudioEngine* engine)
{
	// This function does nothing if we are not working one of the noise channels
	// (`CHAN4` or `CHAN8`).
	if (
		ctx.currentChannelIndex == GSCA_VC_CHAN4 ||
		ctx.currentChannelIndex == GSCA_VC_CHAN8
	)
	{

		// The new note duration should be in the low nibble of the current
		// music byte.
		gscaSetNoteDuration(engine, ctx.currentMusicByte & 0xF);

		// Keep the proper sample index here.
		uint8_t sampleIndex = 0;

		// Figure out which noise sample set to choose from.
		if (ctx.currentChannelIndex == GSCA_VC_CHAN4)
		{
			// If channel 8 is on, then go no further here.
			if (ctx.channels[GSCA_VC_CHAN8].channelOn == 1)
			{
				return;
			}

			sampleIndex = ctx.musicNoiseSampleSet;
		}
		else
		{
			sampleIndex = ctx.musicNoiseSampleSet;
		}

		// Get the note from the upper nibble of the music byte.
		uint8_t note = (ctx.currentMusicByte >> 4);

		// If the note is not a rest note, then use the above-calculated sample
		// index and note values to index the correct drum instrument.
		if (note != 0)
		{
			ctx.noiseSampleAddress = GSCA_DRUMKIT_COLLECTION[sampleIndex][note];

			// Also, reset the noise sample delay.
			ctx.noiseSampleDelay = 0;
		}

	}
}

void gscaParseMusicCommand (gscaAudioEngine* engine)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

	// The current music byte should contain the opcode of the music command.
	uint8_t opcode = ctx.currentMusicByte;

	switch (opcode)
	{
		case GSCA_OCTAVE_CMD0:
		case GSCA_OCTAVE_CMD1:
		case GSCA_OCTAVE_CMD2:
		case GSCA_OCTAVE_CMD3:
		case GSCA_OCTAVE_CMD4:
		case GSCA_OCTAVE_CMD5:
		case GSCA_OCTAVE_CMD6:
		case GSCA_OCTAVE_CMD7:
		{
			channel->octave = ctx.currentMusicByte & 0b111;
		} break;
		case GSCA_NOTE_TYPE_CMD:
		{
			channel->noteLength = gscaGetMusicByte(engine);
			if ((ctx.currentChannelIndex & 0b11) < GSCA_VC_CHAN4)
			{
				channel->volumeEnvelope = gscaGetMusicByte(engine);
			}
		} break;
		case GSCA_TRANSPOSE_CMD:
		{
			channel->transposition = gscaGetMusicByte(engine);
		} break;
		case GSCA_TEMPO_CMD:
		{
			uint16_t high = gscaGetMusicByte(engine);
			uint16_t low  = gscaGetMusicByte(engine);
			gscaSetGlobalTempo(engine, (high << 8) | low);
		} break;
		case GSCA_DUTY_CYCLE_CMD:
		{
			channel->dutyCycle = (gscaGetMusicByte(engine) << 6);
		} break;
		case GSCA_VOLUME_ENVELOPE_CMD:
		{
			channel->volumeEnvelope = gscaGetMusicByte(engine);
		} break;
		case GSCA_PITCH_SWEEP_CMD:
		{
			ctx.pitchSweep.value = gscaGetMusicByte(engine);
			channel->pitchSweep = 1;
		} break;
		case GSCA_DUTY_CYCLE_PATTERN_CMD:
		{
			channel->dutyLoop = 1;
			channel->dutyCyclePattern = gscaGetMusicByte(engine);
			channel->dutyCyclePattern =
				(channel->dutyCyclePattern << 2) |
				(channel->dutyCyclePattern >> 6);
			channel->dutyCycle = (channel->dutyCyclePattern & 0b11000000);
		} break;
		case GSCA_TOGGLE_SFX_CMD:
		{
			channel->sfx ^= 1;
		} break;
		case GSCA_PITCH_SLIDE_CMD:
		{
			ctx.currentNoteDuration = gscaGetMusicByte(engine);
			uint8_t note = gscaGetMusicByte(engine);

			channel->pitchSlideTarget = gscaGetFrequency(engine, note & 0xF, note >> 4);
			channel->pitchSlide = 1;
		} break;
		case GSCA_VIBRATO_CMD:
		{
			channel->vibrato = 1;
			channel->vibratoDir = 0;
			channel->vibratoDelay = gscaGetMusicByte(engine);
			channel->vibratoDelayCount = channel->vibratoDelay;

			uint8_t param = gscaGetMusicByte(engine);
			channel->vibratoRate = (param & 0xF);
			channel->vibratoRate |= (channel->vibratoRate << 4);

			param >>= 4;
			channel->vibratoExtent = (param >> 1);
			channel->vibratoExtent |= ((channel->vibratoExtent + (param & 1)) << 4);
		} break;
		case GSCA_TOGGLE_NOISE_CMD:
		{
			channel->noise ^= 1;
			if (channel->noise == 1)
			{
				ctx.musicNoiseSampleSet = gscaGetMusicByte(engine);
			}
		} break;
		case GSCA_FORCE_STEREO_PANNING_CMD:
		{
			gscaSetLRTracks(engine, ctx.currentChannelIndex);
			channel->tracks &= gscaGetMusicByte(engine);
		} break;
		case GSCA_VOLUME_CMD:
		{
			uint8_t param = gscaGetMusicByte(engine);
			if (ctx.musicFade.value == 0)
			{
				ctx.volume.value = param;
			}
		} break;
		case GSCA_PITCH_OFFSET_CMD:
		{
			uint16_t high = gscaGetMusicByte(engine);
			uint16_t low  = gscaGetMusicByte(engine);

			channel->pitchOffsetEnabled = 1;
			channel->pitchOffset = (high << 8) | low;
		} break;
		case GSCA_TEMPO_RELATIVE_CMD:
		{
			uint8_t param = gscaGetMusicByte(engine);
			if (param & 0x80)
			{
				gscaSetGlobalTempo(engine, channel->tempo - (param & 0x7F));
			}
			else
			{
				gscaSetGlobalTempo(engine, channel->tempo + param);
			}
		} break;
		case GSCA_RESTART_CHANNEL_CMD:
		{
			// Ignore this command; skip over its word parameter.
			gscaGetMusicByte(engine);
			gscaGetMusicByte(engine);
		} break;
		case GSCA_NEW_SONG_CMD:
		{
			// Ignore this command; skip over its word parameter.
			gscaGetMusicByte(engine);
			gscaGetMusicByte(engine);
		} break;
		case GSCA_SFX_PRIORITY_ON_CMD:
		{
			ctx.sfxPriority = 1;
		} break;
		case GSCA_SFX_PRIORITY_OFF_CMD:
		{
			ctx.sfxPriority = 0;
		} break;
		case GSCA_STEREO_PANNING_CMD:
		{
			// TODO: Implement OPTIONS in WRAM!
			if (engine->stereo == true)
			{
				gscaSetLRTracks(engine, ctx.currentChannelIndex);
				channel->tracks &= gscaGetMusicByte(engine);
			}
			else
			{
				gscaGetMusicByte(engine);
			}
		} break;
		case GSCA_SFX_TOGGLE_NOISE_CMD:
		{
			channel->noise ^= 1;
			if (channel->noise == 1)
			{
				ctx.sfxNoiseSampleSet = gscaGetMusicByte(engine);
			}
		} break;
		case GSCA_SET_CONDITION_CMD:
		{
			channel->condition = gscaGetMusicByte(engine);
		} break;
		case GSCA_SOUND_JUMP_IF_CMD:
		{
			if (gscaGetMusicByte(engine) == channel->condition)
			{
                uint64_t    byte0 = gscaGetMusicByte(engine),
                            byte1 = gscaGetMusicByte(engine),
                            byte2 = gscaGetMusicByte(engine),
                            byte3 = gscaGetMusicByte(engine),
                            byte4 = gscaGetMusicByte(engine),
                            byte5 = gscaGetMusicByte(engine),
                            byte6 = gscaGetMusicByte(engine),
                            byte7 = gscaGetMusicByte(engine);
                channel->musicAddress =
                    (byte0 <<  0) |
                    (byte1 <<  8) |
                    (byte2 << 16) |
                    (byte3 << 24) |
                    (byte4 << 32) |
                    (byte5 << 40) |
                    (byte6 << 48) |
                    (byte7 << 56);
			}
			else
			{	
				channel->musicAddress += 8;
			}
		} break;
		case GSCA_SOUND_JUMP_CMD:
		{
            uint64_t    byte0 = gscaGetMusicByte(engine),
                        byte1 = gscaGetMusicByte(engine),
                        byte2 = gscaGetMusicByte(engine),
                        byte3 = gscaGetMusicByte(engine),
                        byte4 = gscaGetMusicByte(engine),
                        byte5 = gscaGetMusicByte(engine),
                        byte6 = gscaGetMusicByte(engine),
                        byte7 = gscaGetMusicByte(engine);
            channel->musicAddress =
                (byte0 <<  0) |
                (byte1 <<  8) |
                (byte2 << 16) |
                (byte3 << 24) |
                (byte4 << 32) |
                (byte5 << 40) |
                (byte6 << 48) |
                (byte7 << 56);
		} break;
		case GSCA_SOUND_LOOP_CMD:
		{
			uint8_t loop_count = gscaGetMusicByte(engine);
			if (channel->looping == 0)
			{
				if (loop_count == 0)
                {
                    uint64_t    byte0 = gscaGetMusicByte(engine),
                                byte1 = gscaGetMusicByte(engine),
                                byte2 = gscaGetMusicByte(engine),
                                byte3 = gscaGetMusicByte(engine),
                                byte4 = gscaGetMusicByte(engine),
                                byte5 = gscaGetMusicByte(engine),
                                byte6 = gscaGetMusicByte(engine),
                                byte7 = gscaGetMusicByte(engine);
                    channel->musicAddress =
                        (byte0 <<  0) |
                        (byte1 <<  8) |
                        (byte2 << 16) |
                        (byte3 << 24) |
                        (byte4 << 32) |
                        (byte5 << 40) |
                        (byte6 << 48) |
                        (byte7 << 56);
					break;
				}

				channel->looping = 1;
				channel->loopCount = loop_count - 1;
			}

			if (channel->loopCount == 0)
			{
				channel->looping = 0;
				channel->loopCount = 0;
				channel->musicAddress += 8;
			}
			else
			{
                uint64_t    byte0 = gscaGetMusicByte(engine),
                            byte1 = gscaGetMusicByte(engine),
                            byte2 = gscaGetMusicByte(engine),
                            byte3 = gscaGetMusicByte(engine),
                            byte4 = gscaGetMusicByte(engine),
                            byte5 = gscaGetMusicByte(engine),
                            byte6 = gscaGetMusicByte(engine),
                            byte7 = gscaGetMusicByte(engine);
                channel->musicAddress =
                    (byte0 <<  0) |
                    (byte1 <<  8) |
                    (byte2 << 16) |
                    (byte3 << 24) |
                    (byte4 << 32) |
                    (byte5 << 40) |
                    (byte6 << 48) |
                    (byte7 << 56);
				channel->loopCount--;
			}
		} break;
		case GSCA_SOUND_CALL_CMD:
		{
            uint64_t    byte0 = gscaGetMusicByte(engine),
                        byte1 = gscaGetMusicByte(engine),
                        byte2 = gscaGetMusicByte(engine),
                        byte3 = gscaGetMusicByte(engine),
                        byte4 = gscaGetMusicByte(engine),
                        byte5 = gscaGetMusicByte(engine),
                        byte6 = gscaGetMusicByte(engine),
                        byte7 = gscaGetMusicByte(engine);
            channel->lastMusicAddress = (channel->musicAddress);
            channel->musicAddress =
                (byte0 <<  0) |
                (byte1 <<  8) |
                (byte2 << 16) |
                (byte3 << 24) |
                (byte4 << 32) |
                (byte5 << 40) |
                (byte6 << 48) |
                (byte7 << 56);

			channel->subroutine = 1;
		} break;
		case GSCA_SOUND_RET_CMD:
		{
			channel->subroutine = 0;
			channel->musicAddress = channel->lastMusicAddress;
		} break;
		default: break;
	}
}

uint8_t gscaGetMusicByte (gscaAudioEngine* engine)
{
    // Read from the engine's data store's buffer at the current music address.
    // Place the byat that was read into the engine context's current music byte.
    const uint64_t size = gscaGetAudioDataSize(engine->store);
    if (size == 0)
    {
        ctx.currentMusicByte = 0;
    }
    else
    {
        gscaChannelStruct* channel = gscaCurrentChannel(engine);
        const uint8_t* data = gscaGetAudioData(engine->store);
        if (channel->musicAddress >= size)
        {
            channel->musicAddress = 0;
        }

        ctx.currentMusicByte = data[channel->musicAddress++];
    }

    return ctx.currentMusicByte;
}

uint16_t gscaGetFrequency (gscaAudioEngine* engine, uint8_t note, uint8_t octave)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

	// Adjust the provided note accordingly to the lower nibble of the current channel's
	// transposition.
	//
	// If the resultant note value wraps around to zero, then return zero frequency.
	note += (channel->transposition & 0xF);
	if (note == 0)
	{
		return 0;
	}

	// Adjust the provided octave value according to the upper nibble of the transposition value.
	// Ensure that the resultant value is capped at a maximum of 7.
	octave += ((channel->transposition >> 4) & 0xF);
	octave  = 7 - ((octave > 7) ? 7 : octave);

	// The resutant note value is used to index a fixed frequency table. To get the actual frequency
	// to be returned...
	// - The indexed frequency is bitwise OR'd with `0xF0000` to accomidate higher frequencies.
	// - The result is shifted right by the resultant octave to get the right note range.
	// - The result is finally wrapped into a range between 0 and 2047 inclusive.
	return (
		(
			(GSCA_FREQUENCY_TABLE[note] | 0xF0000) >> octave
		) & 0x7FF
	);
}

void gscaSetNoteDuration (gscaAudioEngine* engine, uint8_t duration)
{
    gscaChannelStruct* channel = gscaCurrentChannel(engine);

	// Increment the given duration value.
	duration++;

	// Determine the new note duration and fraction from the given duration value.
	uint16_t result = (
		(
			((channel->noteLength * duration) & 0xFF)
		) * channel->tempo
	) + channel->noteDurationFraction;

	// The note duration resides in the upper nibble of the result, whereas the
	// note duration fraction (`field16`) resides in the lower nibble.
	channel->noteDurationFraction	= result & 0xFF;
	channel->noteDuration			= result >> 8;
}

void gscaSetGlobalTempo (gscaAudioEngine* engine, uint16_t tempo)
{
    // Depending on the channel currently being worked on, get the index of
	// either the first music channel or the first SFX channel.
	uint8_t channelIndex =
		(ctx.currentChannelIndex < GSCA_VC_MUSIC_COUNT)
			? 0
			: GSCA_VC_MUSIC_COUNT;

	// Set the tempo of each of the music/SFX channels to the tempo value given.
	// Also, reset their note duration fraction values.
	for (uint8_t i = 0; i < GSCA_VC_MUSIC_COUNT; ++i)
	{	
		ctx.channels[channelIndex + i].tempo = tempo;
		ctx.channels[channelIndex + i].noteDurationFraction = 0;
	}
}

void gscaStartChannel (gscaAudioEngine* engine)
{
    gscaSetLRTracks(engine, ctx.currentChannelIndex);
    gscaCurrentChannel(engine)->channelOn = true;
}

void gscaSetLRTracks (gscaAudioEngine* engine, uint8_t index)
{
    gscaChannelStruct* channel = &ctx.channels[index];
    channel->tracks = (1 << (index & 0b11));
    channel->tracks |= (channel->tracks << 4);
}

void gscaPlayStereoLoadedSFX (gscaAudioEngine* engine, uint16_t id)
{
    // Music off.
    gscaMusicOff(engine);

    // Run the normal routine if stereo is off.
    if (engine->stereo == false)
    {
        gscaPlayLoadedSFX(engine, id);
        return;
    }

    // Set the music ID, then get its offset.
    ctx.musicId = id;
    uint64_t cursor = gscaGetHandleByID(engine->store, ctx.musicId)->offset;

	// The first byte of the music header should be at address $0000. Load it.
	uint8_t firstByte = gscaLoadMusicByte(engine, cursor);
	
	// The channel count should be located in bit 6 of that first byte.
	uint8_t channelCount = (firstByte >> 6);

	// Iterate over and prime the proper channels to play the SFX.
	for (uint8_t i = 0; i <= channelCount; ++i)
	{
        // Load, get, then start the now-current channel. Indicate that it is 
        // playing a sound effect.
		cursor = gscaLoadChannel(engine, cursor);
        gscaChannelStruct* channel = gscaCurrentChannel(engine);
		channel->sfx = 1;
		gscaStartChannel(engine);

        // Configure panning settings for this channel.
        const uint8_t* lrTracks = gscaGetLRTracks(engine);
        uint8_t panning = lrTracks[ctx.currentChannelIndex & 0b11] &
            ctx.stereoPanningMask;
        channel->tracks = panning;
        channel->field30 = (channel->field30 & 0xFF00) | panning;

        // If at least two virtual channels are playing a cry, then a few of
        // this channel's yet-unspecified settings and flags are modified
        // according to the current SFX duration.
        if (ctx.cryTracks >= 2)
        {
            channel->field2e = ctx.sfxDuration;
            channel->field2f = ctx.sfxDuration;
            channel->unknown0f = 1;
        }

        // Channel on.
        channel->channelOn = true;
	}

    // Music on.
    gscaMusicOn(engine);
}

uint64_t gscaLoadChannel (gscaAudioEngine* engine, uint64_t pointer)
{
    // Set the current audio channel.
	ctx.currentChannelIndex = gscaLoadMusicByte(engine, pointer++) & 0b111;
	
	// Point to and initialize the correct audio channel.
	gscaChannelStruct* channel = gscaCurrentChannel(engine);
	channel->channelOn = 0;
    gscaChannelInit(engine, ctx.currentChannelIndex);
	
	// Set the channel's music address to the proper position.
    uint64_t  byte0 = gscaLoadMusicByte(engine, pointer++),
            byte1 = gscaLoadMusicByte(engine, pointer++),
            byte2 = gscaLoadMusicByte(engine, pointer++),
            byte3 = gscaLoadMusicByte(engine, pointer++),
            byte4 = gscaLoadMusicByte(engine, pointer++),
            byte5 = gscaLoadMusicByte(engine, pointer++),
            byte6 = gscaLoadMusicByte(engine, pointer++),
            byte7 = gscaLoadMusicByte(engine, pointer++);

    channel->musicAddress =
        (byte0 <<  0) |
        (byte1 <<  8) |
        (byte2 << 16) |
        (byte3 << 24) |
        (byte4 << 32) |
        (byte5 << 40) |
        (byte6 << 48) |
        (byte7 << 56);

	// Set the channel's music ID.
	channel->musicId = ctx.musicId;

	return pointer;
}

void gscaChannelInit (gscaAudioEngine* engine, uint8_t index)
{
    // Point to the channel.
	gscaChannelStruct* channel = &ctx.channels[index & 0b111];

	// Clear the channel.
	gscaZero(channel, 1, gscaChannelStruct);

	// Set the channel's tempo and note length to their default values (0x100
	// and 0x1, respectively).
	channel->tempo      = 0x100;
	channel->noteLength = 0x1;
}

uint8_t gscaLoadMusicByte (gscaAudioEngine* engine, uint64_t pointer)
{
    // Read from the engine's data store's buffer at the specified pointer.
    // Place the byat that was read into the engine context's current music byte.
    const uint64_t size = gscaGetAudioDataSize(engine->store);
    if (size == 0)
    {
        ctx.currentMusicByte = 0;
    }
    else
    {
        const uint8_t* data = gscaGetAudioData(engine->store);
        if (pointer < size)
        {
            ctx.currentMusicByte = data[pointer];
        }
    }

    return ctx.currentMusicByte;
}

const uint8_t* gscaGetLRTracks (gscaAudioEngine* engine)
{
    static const uint8_t
        STEREO_TRACKS[] = { 0x11, 0x22, 0x44, 0x88 },
        MONO_TRACKS[]   = { 0x11, 0x22, 0x44, 0x88 };

    return (engine->stereo == true) ? STEREO_TRACKS : MONO_TRACKS;
}

void gscaFadeToLoadedMusic (gscaAudioEngine* engine, uint16_t id, uint8_t length)
{
    ctx.musicFade.fadeIn = 0;
    ctx.musicFade.frameCount = (length & 0b111111);
    ctx.musicFadeId = id;
}

void gscaPlayLoadedMusic (gscaAudioEngine* engine, uint16_t id)
{
	// Music off.
    gscaMusicOff(engine);
    gscaInitAudioEngine(engine);

    // Set the music ID and get the appropriate handle.
    ctx.musicId = id;
    
    // Keep track of a memory cursor.
    uint64_t cursor = gscaGetHandleByID(engine->store, ctx.musicId)->offset;

	// The first byte of the music header should be at address $0000. Load it.
	uint8_t firstByte = gscaLoadMusicByte(engine, cursor);
	
	// The channel count should be located in bit 6 of that first byte.
	uint8_t channelCount = (firstByte >> 6) & 0b11;
	for (uint8_t i = 0; i <= channelCount; ++i)
	{
		cursor = gscaLoadChannel(engine, cursor);
		gscaStartChannel(engine);
	}

	// Reset the channels' jump conditions.
	for (uint8_t i = 0; i < GSCA_VC_MUSIC_COUNT; ++i)
	{
		ctx.channelJumpConditions[i] = 0;
	}

	ctx.noiseSampleAddress = NULL;
	ctx.noiseSampleDelay = 0;
	ctx.musicNoiseSampleSet = 0;

    // Music on.
    gscaMusicOn(engine);
}

void gscaPlayLoadedSFX (gscaAudioEngine* engine, uint16_t id)
{
    // Music off.
    gscaMusicOff(engine);

    // Get, then clear, each of the SFX channel structures.
	gscaChannelStruct*  ch5 = &ctx.channels[GSCA_VC_CHAN5];
	gscaChannelStruct*  ch6 = &ctx.channels[GSCA_VC_CHAN6];
	gscaChannelStruct*  ch7 = &ctx.channels[GSCA_VC_CHAN7];
	gscaChannelStruct*  ch8 = &ctx.channels[GSCA_VC_CHAN8];

	if (ch5->channelOn == 1)
	{
		ch5->channelOn = 0;
		gscaClearChannel(engine, GSCA_AC_PULSE1);
		ctx.pitchSweep.value = 0;
	}

	if (ch6->channelOn == 1)
	{
		ch6->channelOn = 0;
		gscaClearChannel(engine, GSCA_AC_PULSE2);
	}

	if (ch7->channelOn == 1)
	{
		ch7->channelOn = 0;
		gscaClearChannel(engine, GSCA_AC_WAVE);
	}

	if (ch8->channelOn == 1)
	{
		ch8->channelOn = 0;
		gscaClearChannel(engine, GSCA_AC_NOISE);
		ctx.noiseSampleAddress = NULL;
	}
	
	// Overload the music ID with the SFX ID.
	ctx.musicId = id;

	// Keep track of a memory cursor.
	uint64_t cursor = gscaGetHandleByID(engine->store, ctx.musicId)->offset;

	// The first byte of the music header should be at address $0000. Load it.
	uint8_t firstByte = gscaLoadMusicByte(engine, cursor);
	
	// The channel count should be located in bit 6 of that first byte.
	uint8_t channelCount = (firstByte >> 6);
    
	// Iterate over and prime the proper channels to play the SFX.
	for (uint8_t i = 0; i <= channelCount; ++i)
	{
		cursor = gscaLoadChannel(engine, cursor);
		ctx.channels[ctx.currentChannelIndex].sfx = 1;
		gscaStartChannel(engine);
	}

    // Music on; SFX priority off.
    gscaMusicOn(engine);
    ctx.sfxPriority = false;
}

void gscaPlayLoadedCry (gscaAudioEngine* engine, uint16_t id)
{
	// Music off.
    gscaMusicOff(engine);
	
	// Overload the music ID with the cry ID.
	ctx.musicId = id;

	// Keep track of a memory cursor.
	uint64_t cursor = gscaGetHandleByID(engine->store, ctx.musicId)->offset;

	// The first byte of the music header should be at address $0000. Load it.
	uint8_t firstByte = gscaLoadMusicByte(engine, cursor);
	
	// The channel count should be located in bit 6 of that first byte.
	uint8_t channelCount = (firstByte >> 6);
	uint8_t channelIndex = 0;
	for (uint8_t i = 0; i <= channelCount; ++i)
	{
		// `gscaLoadChannel` sets the current audio channel.
		cursor = gscaLoadChannel(engine, cursor);
		channelIndex = ctx.currentChannelIndex;
		gscaChannelStruct* channel = &ctx.channels[channelIndex];

		// Set the channel's cry and pitch offset flags.
		channel->cry = 1;
		channel->pitchOffsetEnabled = 1;
		channel->pitchOffset = ctx.cryPitch;

		// Set tempo for this channel if it's not a noise channel.
		if (channelIndex != GSCA_VC_MUSIC4 && channelIndex != GSCA_VC_SFX4)
		{
			channel->tempo = ctx.cryLength;
		}

		// Start the channel.
		gscaStartChannel(engine);

        // Play the cry from the appropriate direction if stereo is enabled.
        if (ctx.stereoPanningMask != 0 && engine->stereo == true)
        {
            channel->tracks &= ctx.cryTracks;
        }
	}

	if (ctx.lastVolume == 0)
	{
		ctx.lastVolume = ctx.volume.value;
		ctx.volume.value = GSCA_MAX_VOLUME;
	}

    // SFX priority, music on.
	ctx.sfxPriority = 1;
    gscaMusicOn(engine);
}

void gscaClearChannels (gscaAudioEngine* engine)
{
    gscaWriteNR52(engine->apu, 0x80);
    gscaWriteNR51(engine->apu, 0x00);
    gscaWriteNR50(engine->apu, 0x00);
    gscaClearChannel(engine, GSCA_AC_PULSE1);
    gscaClearChannel(engine, GSCA_AC_PULSE2);
    gscaClearChannel(engine, GSCA_AC_WAVE);
    gscaClearChannel(engine, GSCA_AC_NOISE);
}

void gscaClearChannel (gscaAudioEngine* engine, gscaAudioChannel channel)
{
    switch (channel)
    {
        case GSCA_AC_PULSE1:
            gscaWriteNR10(engine->apu, 0x00);
            gscaWriteNR11(engine->apu, 0x00);
            gscaWriteNR12(engine->apu, 0x08);
            gscaWriteNR13(engine->apu, 0x00);
            gscaWriteNR14(engine->apu, 0x80);
            break;
        case GSCA_AC_PULSE2:
            gscaWriteNR21(engine->apu, 0x00);
            gscaWriteNR22(engine->apu, 0x08);
            gscaWriteNR23(engine->apu, 0x00);
            gscaWriteNR24(engine->apu, 0x80);
            break;
        case GSCA_AC_WAVE:
            gscaWriteNR30(engine->apu, 0x00);
            gscaWriteNR31(engine->apu, 0x00);
            gscaWriteNR32(engine->apu, 0x08);
            gscaWriteNR33(engine->apu, 0x00);
            gscaWriteNR34(engine->apu, 0x80);
            break;
        case GSCA_AC_NOISE:
            gscaWriteNR41(engine->apu, 0x00);
            gscaWriteNR42(engine->apu, 0x08);
            gscaWriteNR43(engine->apu, 0x00);
            gscaWriteNR44(engine->apu, 0x80);
            break;
    }
}

/* Public Functions ***********************************************************/

gscaAudioEngine* gscaCreateAudioEngine (gscaAPU* apu, gscaAudioStore* audioStore)
{
    gscaExpect(apu, "Pointer 'apu' is NULL!\n");
    gscaExpect(apu, "Pointer 'audioStore' is NULL!\n");

    gscaAudioEngine* engine = gscaCreateZero(1, gscaAudioEngine);
    gscaExpectp(engine, "Could not allocate audio engine structure");
    
    engine->apu = apu;
    engine->store = audioStore;
    gscaInitAudioEngine(engine);

    return engine;
}

void gscaDestroyAudioEngine (gscaAudioEngine* engine)
{
    if (engine != NULL)
    {
        engine->apu = NULL;
        engine->store = NULL;
        gscaDestroy(engine);
    }
}

void gscaInitAudioEngine (gscaAudioEngine* engine)
{
    gscaExpect(engine, "Pointer 'engine' is NULL!\n");

    gscaMusicOff(engine);
    gscaClearChannels(engine);
    gscaZero(&engine->context, 1, engine->context);

    ctx.volume.value        = GSCA_MAX_VOLUME;
    ctx.noiseSampleAddress  = NULL;

    gscaMusicOn(engine);
}

void gscaUpdateAudioEngine (gscaAudioEngine* engine)
{
    gscaExpect(engine, "Pointer 'engine' is NULL!\n");

    // Don't bother if the engine is turned off.
    if (engine->musicPlaying == false)
    {
        return;
    }

    // Reset the sound output from the last update.
    ctx.soundOutput.value = 0x00;

    // Iterate over each virtual channel and update each.
    for (uint8_t i = 0; i < GSCA_VC_COUNT; ++i)
    {
        // Update the current channel index, then get that channel.
        ctx.currentChannelIndex = i;
        gscaChannelStruct* channel = gscaCurrentChannel(engine);

        // Update the channel only if it's on.
        if (channel->channelOn == true)
        {
            // Check the time remaining on the current note. It's done if that
            // time is less than two.
            if (channel->noteDuration < 2)
            {
                // If it is done, then prepare the channel for parsing the next
                // note.
                channel->vibratoDelayCount = channel->vibratoDelay;
                channel->pitchSlide = 0;
                gscaParseMusic(engine);
            }
            else
            {
                channel->noteDuration--;
            }
            
            // Apply the pitch slide, if needed.
            gscaApplyPitchSlide(engine);
            
            // Store the current channel's duty, envelope and frequency settings in
            // the engine's context.
            ctx.currentTrackDuty = channel->dutyCycle;
            ctx.currentTrackEnvelope = channel->volumeEnvelope;
            ctx.currentTrackFrequency = channel->frequency;
            
            // Handle vibrato/noise.
            gscaHandleTrackVibrato(engine);
            gscaHandleNoise(engine);
            
            // If the SFX channels have priority, and an SFX channel is playing
            // audio, then rest this channel.
            if (ctx.sfxPriority == true && i < GSCA_VC_MUSIC_COUNT)
            {
                for (uint8_t j = GSCA_VC_MUSIC_COUNT; j < GSCA_VC_COUNT; ++j)
                {
                    if (ctx.channels[j].channelOn == true)
                        { channel->rest = true; break; }
                }
            }
            
            // If the engine is working on an SFX channel, or if a music channel is
            // being processed and its coresponding SFX channel is off, then update
            // this channel.
            if (
                (i >= GSCA_VC_MUSIC_COUNT) ||
                (ctx.channels[i + GSCA_VC_MUSIC_COUNT].channelOn == false)
            )
            {
                gscaUpdateChannel(engine);
                ctx.soundOutput.value |= channel->tracks;
            }
            
            // Clear the channel's note 
            channel->noteFlags = 0;
        }
    }

    gscaPlayDangerTone(engine);     // Play the low health alarm, if needed.
    gscaFadeMusic(engine);          // Fade music, if needed.

    // Write the engine's volume and panning configs to the APU.
    gscaWriteNR50(engine->apu, ctx.volume.value);
    gscaWriteNR51(engine->apu, ctx.soundOutput.value);
}

int32_t gscaIsPlayingSFX (const gscaAudioEngine* engine)
{
    for (int32_t i = GSCA_VC_MUSIC_COUNT; i < GSCA_VC_COUNT; ++i)
    {
        if (ctx.channels[i].channelOn == true)
        {
            return i;
        }
    }

    return 0;
}

bool gscaFadeToMusic (gscaAudioEngine* engine, const char* name, uint8_t length)
{
    gscaExpect(engine, "Pointer 'engine' is NULL!\n");
    gscaExpect(name, "Pointer 'name' is NULL!\n");

    if (name[0] == '\0')
    {
        gscaErr("Audio handle name cannot be blank.\n");
        return false;
    }

    const gscaAudioHandle* handle = gscaGetHandleByName(engine->store, name);
    if (handle == NULL)
    {
        gscaErr("Audio handle '%s' not found.\n", name);
        return false;
    }

    gscaFadeToLoadedMusic(engine, handle->id, length);
    return true;
}

bool gscaPlayMusic (gscaAudioEngine* engine, const char* name)
{
    gscaExpect(engine, "Pointer 'engine' is NULL!\n");
    gscaExpect(name, "Pointer 'name' is NULL!\n");

    if (name[0] == '\0')
    {
        gscaErr("Audio handle name cannot be blank.\n");
        return false;
    }

    const gscaAudioHandle* handle = gscaGetHandleByName(engine->store, name);
    if (handle == NULL)
    {
        gscaErr("Audio handle '%s' not found.\n", name);
        return false;
    }

    gscaPlayLoadedMusic(engine, handle->id);
    return true;
}

bool gscaPlaySFX (gscaAudioEngine* engine, const char* name)
{
    gscaExpect(engine, "Pointer 'engine' is NULL!\n");
    gscaExpect(name, "Pointer 'name' is NULL!\n");

    if (name[0] == '\0')
    {
        gscaErr("Audio handle name cannot be blank.\n");
        return false;
    }

    const gscaAudioHandle* handle = gscaGetHandleByName(engine->store, name);
    if (handle == NULL)
    {
        gscaErr("Audio handle '%s' not found.\n", name);
        return false;
    }

    gscaPlayLoadedSFX(engine, handle->id);
    return true;
}

bool gscaPlayStereoSFX (gscaAudioEngine* engine, const char* name)
{
    gscaExpect(engine, "Pointer 'engine' is NULL!\n");
    gscaExpect(name, "Pointer 'name' is NULL!\n");

    if (name[0] == '\0')
    {
        gscaErr("Audio handle name cannot be blank.\n");
        return false;
    }

    const gscaAudioHandle* handle = gscaGetHandleByName(engine->store, name);
    if (handle == NULL)
    {
        gscaErr("Audio handle '%s' not found.\n", name);
        return false;
    }

    gscaPlayStereoLoadedSFX(engine, handle->id);
    return true;    
}

bool gscaPlayCry (gscaAudioEngine* engine, const char* name, int16_t pitch, int16_t length)
{
    gscaExpect(engine, "Pointer 'engine' is NULL!\n");
    gscaExpect(name, "Pointer 'name' is NULL!\n");

    if (name[0] == '\0')
    {
        gscaErr("Audio handle name cannot be blank.\n");
        return false;
    }

    const gscaAudioHandle* handle = gscaGetHandleByName(engine->store, name);
    if (handle == NULL)
    {
        gscaErr("Audio handle '%s' not found.\n", name);
        return false;
    }

    ctx.cryPitch = (uint16_t) pitch;
    ctx.cryLength = (uint16_t) length;

    gscaPlayLoadedCry(engine, handle->id);
    return true;
}

#undef ctx
