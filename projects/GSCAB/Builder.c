/**
 * @file    GSCAB/Builder.c
 */

#include <GSCA/Commands.h>
#include <GSCA/AudioStore.h>
#include <GSCAB/Lexer.h>
#include <GSCAB/Builder.h>

/* Private Constants **********************************************************/

#define GSCAB_AS_MAGIC_NUMBER   0x41435347

/* Label Structure ************************************************************/

typedef struct
{
    char    name[GSCAB_LABEL_LEN];
    size_t  offset;
    bool    start;
} gscabLabel;

/* Builder Context Structure **************************************************/

static struct
{
    char        parentLabelText[GSCAB_LEXEME_LEN];
    gscabLabel* labels;
    size_t      labelCount;
    size_t      labelCapacity;
    uint8_t*    binary;
    size_t      binarySize;
    size_t      binaryPointer;
    uint16_t    audioCount;
    uint8_t     channelCount;
} builder = {
    .labels         = nullptr,
    .labelCount     = 0,
    .labelCapacity  = 0,
    .binary         = nullptr,
    .binarySize     = 0,
    .binaryPointer  = 0,
    .audioCount     = 0,
    .channelCount   = 0
};

/* Private Functions **********************************************************/

static void gscabResizeLabels ()
{
    if (builder.labelCount + 1 >= builder.labelCapacity)
    {
        size_t      labelCapacity = builder.labelCapacity * 1.5f;
        gscabLabel* labels = gscaResize(builder.labels, labelCapacity, gscabLabel);
        gscaExpectp(labels, "Could not resize labels array");

        builder.labels = labels;
        builder.labelCapacity = labelCapacity;
    }
}

static void gscabAddLabel (const char* lexeme)
{
    static char resolved[GSCAB_LABEL_LEN] = { 0 };
    bool child = (lexeme[0] == '.');

    if (child == true)
    {
        strncpy(resolved, builder.parentLabelText, GSCAB_LEXEME_LEN);
        strncat(resolved, lexeme, GSCAB_LEXEME_LEN);
    }
    else
    {
        strncpy(resolved, lexeme, GSCAB_LEXEME_LEN);
        strncpy(builder.parentLabelText, lexeme, GSCAB_LEXEME_LEN);
    }

    gscabResizeLabels();
    gscabLabel* label = &builder.labels[builder.labelCount++];
    strncpy(label->name, resolved, GSCAB_LABEL_LEN);
    label->offset = builder.binarySize;
    label->start = false;
    
    if (child == false)
    {
        const gscabToken* peek = gscabTokenAt(0);
        if (
            peek != nullptr &&
            peek->type == GSCAB_TT_COMMAND &&
            peek->param == GSCA_CHANNEL_COUNT_CMD
        )
        {
            label->start = true;
            builder.audioCount++;
        }
    }
}

static size_t gscabResolveLabel (const char* lexeme)
{
    static char resolved[GSCAB_LABEL_LEN] = { 0 };
    bool child = (lexeme[0] == '.');

    if (child == true)
    {
        strncpy(resolved, builder.parentLabelText, GSCAB_LEXEME_LEN);
        strncat(resolved, lexeme, GSCAB_LEXEME_LEN);
    }
    else
    {
        strncpy(resolved, lexeme, GSCAB_LEXEME_LEN);
    }

    for (size_t i = 0; i < builder.labelCount; ++i)
    {
        const gscabLabel* label = &builder.labels[i];
        if (strncmp(label->name, resolved, GSCAB_LABEL_LEN) == 0)
        {
            return label->offset;
        }
    }

    return GSCAB_NPOS;
}

static bool gscabWriteByte (FILE* fp, const uint8_t value)
{
    fwrite(&value, sizeof(uint8_t), 1, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing byte");
        return false;
    }

    return true;
}

static bool gscabWriteWord (FILE* fp, const uint16_t value)
{
    static uint8_t writeWordBytes[2] = { 0 };
    writeWordBytes[0] = ((value >> 0) & 0xFF);
    writeWordBytes[1] = ((value >> 8) & 0xFF);

    fwrite(writeWordBytes, sizeof(uint8_t), 2, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing word");
        return false;
    }

    return true;
}

static bool gscabWriteDoubleWord (FILE* fp, const uint32_t value)
{
    static uint8_t writeDoubleWordBytes[4] = { 0 };
    writeDoubleWordBytes[0] = ((value >>  0) & 0xFF);
    writeDoubleWordBytes[1] = ((value >>  8) & 0xFF);
    writeDoubleWordBytes[2] = ((value >> 16) & 0xFF);
    writeDoubleWordBytes[3] = ((value >> 24) & 0xFF);

    fwrite(writeDoubleWordBytes, sizeof(uint8_t), 4, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing double word");
        return false;
    }

    return true;
}

static bool gscabWriteQuadWord (FILE* fp, const uint64_t value)
{
    static uint8_t writeQuadWordBytes[8] = { 0 };
    writeQuadWordBytes[0] = ((value >>  0) & 0xFF);
    writeQuadWordBytes[1] = ((value >>  8) & 0xFF);
    writeQuadWordBytes[2] = ((value >> 16) & 0xFF);
    writeQuadWordBytes[3] = ((value >> 24) & 0xFF);
    writeQuadWordBytes[4] = ((value >> 32) & 0xFF);
    writeQuadWordBytes[5] = ((value >> 40) & 0xFF);
    writeQuadWordBytes[6] = ((value >> 48) & 0xFF);
    writeQuadWordBytes[7] = ((value >> 56) & 0xFF);

    fwrite(writeQuadWordBytes, sizeof(uint8_t), 8, fp);
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing quad word");
        return false;
    }

    return true;
}

static bool gscabWriteString (FILE* fp, const char* string, size_t length)
{
    fwrite(string, sizeof(char), length, fp);   // Plus null terminator.
    if (ferror(fp))
    {
        gscaErrp("Write error occured while writing string");
        return false;
    }

    return true;
}

static bool gscabPushByte (const uint8_t value)
{
    if (builder.binaryPointer + 1 > builder.binarySize)
    {
        gscaErr("Not enough room to push byte.\n");
        return false;
    }

    builder.binary[builder.binaryPointer++] = value;
    return true;
}

static bool gscabPushWord (const uint16_t value)
{
    if (builder.binaryPointer + 2 > builder.binarySize)
    {
        gscaErr("Not enough room to push word.\n");
        return false;
    }

    builder.binary[builder.binaryPointer++] = ((value >>  0) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >>  8) & 0xFF);
    return true;
}

static bool gscabPushWordBE (const uint16_t value)
{
    if (builder.binaryPointer + 2 > builder.binarySize)
    {
        gscaErr("Not enough room to push word.\n");
        return false;
    }

    builder.binary[builder.binaryPointer++] = ((value >>  8) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >>  0) & 0xFF);
    return true;
}

static bool gscabPushDoubleWord (const uint32_t value)
{
    if (builder.binaryPointer + 4 > builder.binarySize)
    {
        gscaErr("Not enough room to push double word.\n");
        return false;
    }

    builder.binary[builder.binaryPointer++] = ((value >>  0) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >>  8) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 16) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 24) & 0xFF);
    return true;
}

static bool gscabPushQuadWord (const uint64_t value)
{
    if (builder.binaryPointer + 8 > builder.binarySize)
    {
        gscaErr("Not enough room to push quad word.\n");
        return false;
    }

    builder.binary[builder.binaryPointer++] = ((value >>  0) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >>  8) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 16) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 24) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 32) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 40) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 48) & 0xFF);
    builder.binary[builder.binaryPointer++] = ((value >> 56) & 0xFF);
    return true;
}

#define numberArgument(i) \
    const gscabToken* p##i = gscabNextToken(); \
    if (p##i == nullptr) \
    { \
        gscaErr("Unexpected end of token stream while parsing numeric argument #%u.\n", i); \
        return false; \
    } \
    if ( \
        p##i->type != GSCAB_TT_INTEGER && \
        p##i->type != GSCAB_TT_HEXADECIMAL && \
        p##i->type != GSCAB_TT_BOOLEAN && \
        p##i->type != GSCAB_TT_NOTE \
    ) \
    { \
        gscaErr("Expected numeric token for argument #%u.\n", i); \
        return false; \
    } \
    uint64_t u##i = gscabGetInteger(p##i); \
    int64_t s##i = gscabGetSignedInteger(p##i);
#define labelArgument(i) \
    const gscabToken* p##i = gscabNextToken(); \
    if (p##i == nullptr) \
    { \
        gscaErr("Unexpected end of token stream while parsing label argument #%u.\n", i); \
        return false; \
    } \
    if ( \
        p##i->type != GSCAB_TT_IDENTIFIER \
    ) \
    { \
        gscaErr("Expected identifier token for label argument #%u.\n", i); \
        return false; \
    } \
    size_t u##i = gscabResolveLabel(p##i->lexeme); \
    if (u##i == GSCAB_NPOS) \
    { \
        gscaErr("Label argument #%u ('%s') is not defined.\n", i, p##i->lexeme); \
        return false; \
    }
#define expectComma(i) \
    if (gscabExpectToken(GSCAB_TT_COMMA) == nullptr) \
    { \
        gscaErr("Expected ',' after argument #%u.\n", i); \
        return false; \
    }

static bool gscabParseData (const gscabToken* token)
{
    numberArgument(1)
    switch (token->param)
    {
        case GSCAB_DT_BYTE:         return gscabPushByte(u1 & 0xFF);
        case GSCAB_DT_WORD:         return gscabPushWord(u1 & 0xFFFF);
        case GSCAB_DT_DOUBLE_WORD:  return gscabPushDoubleWord(u1 & 0xFFFFFFFF);
        case GSCAB_DT_QUAD_WORD:    return gscabPushQuadWord(u1);
        default:                    return false;
    }
}

static bool gscabParseChannelCount ()
{
    numberArgument(1)

    if (u1 < 1 || u1 > 4)
    {
        gscaErr("Channel count must be between 1 and 4.");
        return false;
    }

    builder.channelCount = (u1 & 0xFF) - 1;
    return true;
}

static bool gscabParseChannel ()
{
    numberArgument(1) expectComma(1)
    labelArgument(2)

    if (u1 < 1 || u1 > 8)
    {
        gscaErr("Channel ID must be between 1 and 8.");
        return false;
    }

    uint8_t byte = ((builder.channelCount << 6) | ((u1 - 1) & 0xF));

    bool ok =
        gscabPushByte((builder.channelCount << 6) | ((u1 - 1) & 0xF)) &&
        gscabPushQuadWord(u2);

    builder.channelCount = 0;

    return ok;
}

static bool gscabParseNote ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2)

    return
        gscabPushByte(
            ((u1 & 0xF) << 4) |
            ((u2 - 1) & 0xF)
        );
}

static bool gscabParseDrumNote ()
{
    return gscabParseNote();
}

static bool gscabParseRest ()
{
    numberArgument(1)

    return
        gscabPushByte((u1 - 1) & 0xF);
}

static bool gscabParseSquareNote ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2) expectComma(2)
    numberArgument(3) expectComma(3)
    numberArgument(4)

    return
        gscabPushByte(u1 & 0xFF) &&
        (
            (s3 < 0) ?
                gscabPushByte(((u2 & 0xF) << 4) | (0b1000 | (s3 * -1))) :
                gscabPushByte(((u2 & 0xF) << 4) | (u3 & 0xF))
        ) &&
        gscabPushWord(u4 & 0xFFFF);
}

static bool gscabParseNoiseNote ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2) expectComma(2)
    numberArgument(3) expectComma(3)
    numberArgument(4)

    return
        gscabPushByte(u1 & 0xFF) &&
        (
            (s3 < 0) ?
                gscabPushByte(((u2 & 0xF) << 4) | (0b1000 | (s3 * -1))) :
                gscabPushByte(((u2 & 0xF) << 4) | (u3 & 0xF))
        ) &&
        gscabPushByte(u4 & 0xFF);
}

static bool gscabParseOctave ()
{
    numberArgument(1)
    if (u1 < 1 || u1 > 8)
    {
        gscaErr("Octave must be between 1 and 8.\n");
        return false;
    }

    return gscabPushByte(GSCA_OCTAVE_CMD + 8 - u1);
}

static bool gscabParseNoteType ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2) expectComma(2)
    numberArgument(3)

    return
        gscabPushByte(GSCA_NOTE_TYPE_CMD) &&
        gscabPushByte(u1 & 0xFF) &&
        (
            (s3 < 0) ?
                gscabPushByte(((u2 & 0xF) << 4) | (0b1000 | ((s3 * -1) & 0xF))) :
                gscabPushByte(((u2 & 0xF) << 4) | (u3 & 0xF))
        );
}

static bool gscabParseDrumSpeed ()
{
    numberArgument(1)

    return
        gscabPushByte(GSCA_NOTE_TYPE_CMD) &&
        gscabPushByte(u1 & 0xFF);
}

static bool gscabParseTranspose ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2)

    return
        gscabPushByte(GSCA_TRANSPOSE_CMD) &&
        gscabPushByte(
            ((u1 & 0xF) << 4) |
            (u2 & 0xF)
        );
}

static bool gscabParseTempo ()
{
    numberArgument(1)

    return
        gscabPushByte(GSCA_TEMPO_CMD) &&
        gscabPushWordBE(u1 & 0xFFFF);
}

static bool gscabParseDutyCycle ()
{
    numberArgument(1)

    return
        gscabPushByte(GSCA_DUTY_CYCLE_CMD) &&
        gscabPushByte(u1 & 0xFF);
}

static bool gscabParseVolumeEnvelope ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2)

    return
        gscabPushByte(GSCA_VOLUME_ENVELOPE_CMD) &&
        (
            (s2 < 0) ?
                gscabPushByte(((u1 & 0xF) << 4) | (0b1000 | (s2 * -1))) :
                gscabPushByte(((u1 & 0xF) << 4) | (u2 & 0xF))
        );
}

static bool gscabParsePitchSweep ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2)

    return
        gscabPushByte(GSCA_PITCH_SWEEP_CMD) &&
        (
            (s2 < 0) ?
                gscabPushByte(((u1 & 0xF) << 4) | (0b1000 | ((s2 * -1) & 0xF))) :
                gscabPushByte(((u1 & 0xF) << 4) | (u2 & 0xF))
        );
}

static bool gscabParseDutyCyclePattern ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2) expectComma(2)
    numberArgument(3) expectComma(3)
    numberArgument(4)

    return
        gscabPushByte(GSCA_DUTY_CYCLE_PATTERN_CMD) &&
        gscabPushByte(
            ((u1 & 0b11) << 6) |
            ((u2 & 0b11) << 4) |
            ((u3 & 0b11) << 2) |
            (u4 & 0b11)
        );
}

static bool gscabParseToggleSfx ()
{
    return 
        gscabPushByte(GSCA_TOGGLE_SFX_CMD);
}

static bool gscabParsePitchSlide ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2) expectComma(2)
    numberArgument(3)

    return
        gscabPushByte(GSCA_PITCH_SLIDE_CMD) &&
        gscabPushByte((u1 & 0xFF) - 1) &&
        gscabPushByte(
            ((8 - (u2 & 0xFF)) << 4) |
            ((u3 & 0xFF) % 12)
        );
}

static bool gscabParseVibrato ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2) expectComma(2)
    numberArgument(3)

    return
        gscabPushByte(GSCA_VIBRATO_CMD) &&
        gscabPushByte(u1 & 0xFF) &&
        gscabPushByte(
            ((u2 & 0xF) << 4) |
            (u3 & 0xF)
        );
}

static bool gscabParseToggleNoise ()
{
    numberArgument(1)

    return
        gscabPushByte(GSCA_TOGGLE_NOISE_CMD) &&
        gscabPushByte(u1 & 0xFF);
}

static bool gscabParseForceStereoPanning ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2)

    return
        gscabPushByte(GSCA_FORCE_STEREO_PANNING_CMD) &&
        gscabPushByte(
            ((0b1111 * (1 && u1)) << 4) |
            (0b1111 * (1 && u2))
        );
}

static bool gscabParseVolume ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2)

    return
        gscabPushByte(GSCA_VOLUME_CMD) &&
        gscabPushByte(
            ((u1 & 0xF) << 4) |
            (u2 & 0xF)
        );
}

static bool gscabParsePitchOffset ()
{
    numberArgument(1)
    
    return
        gscabPushByte(GSCA_PITCH_OFFSET_CMD) &&
        gscabPushWordBE(u1 & 0xFFFF);
}

static bool gscabParseTempoRelative ()
{
    numberArgument(1)
    
    return
        gscabPushByte(GSCA_TEMPO_RELATIVE_CMD) &&
        gscabPushByte(u1 & 0xFF);
}

static bool gscabParseRestartChannel ()
{
    numberArgument(1)
    
    return
        gscabPushByte(GSCA_RESTART_CHANNEL_CMD) &&
        gscabPushQuadWord(u1);
}

static bool gscabParseNewSong ()
{
    numberArgument(1)
    
    return
        gscabPushByte(GSCA_NEW_SONG_CMD) &&
        gscabPushWord(u1 & 0xFFFF);
}

static bool gscabParseSfxPriorityOn ()
{
    return 
        gscabPushByte(GSCA_SFX_PRIORITY_ON_CMD);
}

static bool gscabParseSfxPriorityOff ()
{
    return 
        gscabPushByte(GSCA_SFX_PRIORITY_OFF_CMD);
}

static bool gscabParseStereoPanning ()
{
    numberArgument(1) expectComma(1)
    numberArgument(2)

    return
        gscabPushByte(GSCA_STEREO_PANNING_CMD) &&
        gscabPushByte(
            ((0b1111 * (1 && u1)) << 4) |
            (0b1111 * (1 && u2))
        );
}

static bool gscabParseSfxToggleNoise ()
{
    numberArgument(1)

    return
        gscabPushByte(GSCA_SFX_TOGGLE_NOISE_CMD) &&
        gscabPushByte(u1 & 0xFF);
}

static bool gscabParseSetCondition ()
{
    numberArgument(1)
    
    return
        gscabPushByte(GSCA_SET_CONDITION_CMD) &&
        gscabPushByte(u1 & 0xFF);
}

static bool gscabParseSoundJumpIf ()
{
    numberArgument(1) expectComma(1)
    labelArgument(2)

    return
        gscabPushByte(GSCA_SOUND_LOOP_CMD) &&
        gscabPushByte(u1 & 0xFF) &&
        gscabPushQuadWord(u2);
}

static bool gscabParseSoundJump ()
{
    labelArgument(1)
    return
        gscabPushByte(GSCA_SOUND_JUMP_CMD) &&
        gscabPushQuadWord(u1);
}

static bool gscabParseSoundLoop ()
{
    numberArgument(1) expectComma(1)
    labelArgument(2)

    return
        gscabPushByte(GSCA_SOUND_LOOP_CMD) &&
        gscabPushByte(u1 & 0xFF) &&
        gscabPushQuadWord(u2);
}

static bool gscabParseSoundCall ()
{
    labelArgument(1)
    return
        gscabPushByte(GSCA_SOUND_CALL_CMD) &&
        gscabPushQuadWord(u1);
}

static bool gscabParseSoundRet ()
{
    return gscabPushByte(GSCA_SOUND_RET_CMD);
}

static bool gscabParseTogglePerfectPitch ()
{
    return true;
}

static bool gscabParseCommand (const gscabToken* token)
{
    switch (token->param)
    {
        case GSCA_CHANNEL_COUNT_CMD: return gscabParseChannelCount();
        case GSCA_CHANNEL_CMD: return gscabParseChannel();
        case GSCA_NOTE_CMD: return gscabParseNote();
        case GSCA_DRUM_NOTE_CMD: return gscabParseDrumNote();
        case GSCA_REST_CMD: return gscabParseRest();
        case GSCA_SQUARE_NOTE_CMD: return gscabParseSquareNote();
        case GSCA_NOISE_NOTE_CMD: return gscabParseNoiseNote();
        case GSCA_OCTAVE_CMD: return gscabParseOctave();
        case GSCA_NOTE_TYPE_CMD: return gscabParseNoteType();
        case GSCA_DRUM_SPEED_CMD: return gscabParseDrumSpeed();
        case GSCA_TRANSPOSE_CMD: return gscabParseTranspose();
        case GSCA_TEMPO_CMD: return gscabParseTempo();
        case GSCA_DUTY_CYCLE_CMD: return gscabParseDutyCycle();
        case GSCA_VOLUME_ENVELOPE_CMD: return gscabParseVolumeEnvelope();
        case GSCA_PITCH_SWEEP_CMD: return gscabParsePitchSweep();
        case GSCA_DUTY_CYCLE_PATTERN_CMD: return gscabParseDutyCyclePattern();
        case GSCA_TOGGLE_SFX_CMD: return gscabParseToggleSfx();
        case GSCA_PITCH_SLIDE_CMD: return gscabParsePitchSlide();
        case GSCA_VIBRATO_CMD: return gscabParseVibrato();
        case GSCA_TOGGLE_NOISE_CMD: return gscabParseToggleNoise();
        case GSCA_FORCE_STEREO_PANNING_CMD: return gscabParseForceStereoPanning();
        case GSCA_VOLUME_CMD: return gscabParseVolume();
        case GSCA_PITCH_OFFSET_CMD: return gscabParsePitchOffset();
        case GSCA_TEMPO_RELATIVE_CMD: return gscabParseTempoRelative();
        case GSCA_RESTART_CHANNEL_CMD: return gscabParseRestartChannel();
        case GSCA_NEW_SONG_CMD: return gscabParseNewSong();
        case GSCA_SFX_PRIORITY_ON_CMD: return gscabParseSfxPriorityOn();
        case GSCA_SFX_PRIORITY_OFF_CMD: return gscabParseSfxPriorityOff();
        case GSCA_STEREO_PANNING_CMD: return gscabParseStereoPanning();
        case GSCA_SFX_TOGGLE_NOISE_CMD: return gscabParseSfxToggleNoise();
        case GSCA_SET_CONDITION_CMD: return gscabParseSetCondition();
        case GSCA_SOUND_JUMP_IF_CMD: return gscabParseSoundJumpIf();
        case GSCA_SOUND_JUMP_CMD: return gscabParseSoundJump();
        case GSCA_SOUND_LOOP_CMD: return gscabParseSoundLoop();
        case GSCA_SOUND_CALL_CMD: return gscabParseSoundCall();
        case GSCA_SOUND_RET_CMD: return gscabParseSoundRet();
        case GSCA_TOGGLE_PERFECT_PITCH_CMD: return gscabParseTogglePerfectPitch();
        default: return true;
    }
}

#undef numberArgument
#undef labelArgument
#undef expectComma

/* Public Functions ***********************************************************/

void gscabInitBuilder ()
{
    builder.labels = gscaCreate(GSCAB_DEFAULT_CAPACITY, gscabLabel);
    gscaExpectp(builder.labels, "Could not allocate labels array");

    builder.labelCapacity = GSCAB_DEFAULT_CAPACITY;
}

void gscabShutdownBuilder ()
{
    gscaDestroy(builder.binary);
    gscaDestroy(builder.labels);
}

bool gscabBuilderPassOne ()
{
    gscabResetPointer();
    while (true)
    {
        const gscabToken* token = gscabNextToken();
        if (token == nullptr) { break; }

        if (
            token->type == GSCAB_TT_IDENTIFIER &&
            gscabExpectToken(GSCAB_TT_COLON) != nullptr
        )
        {
            gscabExpectToken(GSCAB_TT_COLON);
            gscabAddLabel(token->lexeme);
        }

        builder.binarySize += token->size;
    }

    builder.binary = gscaCreateZero(builder.binarySize, uint8_t);
    gscaExpectp(builder.binary, "Could not allocate binary output data");

    return true;
}

bool gscabBuilderPassTwo ()
{
    gscabResetPointer();
    while (true)
    {
        const gscabToken* token = gscabNextToken();
        if (token == nullptr) { break; }

        if (token->type == GSCAB_TT_DATA)
        {
            if (gscabParseData(token) == false)
            {
                gscaErr("At offset '$%016lX'.\n", builder.binaryPointer);
                return false;
            }
        }
        else if (token->type == GSCAB_TT_COMMAND)
        { 
            if (gscabParseCommand(token) == false)
            {
                gscaErr("At offset '$%016lX'.\n", builder.binaryPointer);
                return false;
            }
        }
        else if (
            token->type == GSCAB_TT_IDENTIFIER &&
            token->lexeme[0] != '.' &&
            gscabExpectToken(GSCAB_TT_COLON) != nullptr
        )
        {
            strncpy(builder.parentLabelText, token->lexeme, GSCAB_LEXEME_LEN);
        }
    }

    return true;
}

bool gscabSaveBuilderOutput (const char* filename)
{
    FILE* fp = fopen(filename, "wb");
    if (fp == nullptr)
    {
        gscaErrp("Could not open output file '%s' for writing", filename);
        return false;
    }

    // Write header.
    if (
        gscabWriteDoubleWord(fp, GSCAB_AS_MAGIC_NUMBER) == false ||
        gscabWriteByte(fp, GSCA_MAJOR_VERSION) == false ||
        gscabWriteByte(fp, GSCA_MINOR_VERSION) == false ||
        gscabWriteWord(fp, builder.audioCount) == false
    )
    {
        gscaErr("Could not write header to file '%s'.\n", filename);
        fclose(fp);
        return false;
    }

    // Write audio entry table.
    for (size_t i = 0; i < builder.labelCount; ++i)
    {
        const gscabLabel* label = &builder.labels[i];
        if (label->start == false) { continue; }
        
        if (
            gscabWriteString(fp, label->name, GSCA_AS_HANDLE_NAME_STRLEN) == false ||
            gscabWriteQuadWord(fp, label->offset) == false
        )
        {
            gscaErr("Could not write entry #%zu (%s) to entry table of file '%s'.\n",
                i, label->name, filename);
            fclose(fp);
            return false;
        }
    }

    // Write audio data.
    fwrite(builder.binary, sizeof(uint8_t), builder.binarySize, fp);
    fclose(fp);

    return true;
}
