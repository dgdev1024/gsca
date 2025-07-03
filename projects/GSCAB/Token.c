/**
 * @file    GSCAB/Token.c
 */

#include <GSCAB/Token.h>

/* Private Constants **********************************************************/

static const gscabToken KEYWORD_LOOKUP[] = {
    { "true",                   GSCAB_TT_BOOLEAN,   1,                              0 },
    { "false",                  GSCAB_TT_BOOLEAN,   0,                              0 },
    { "c_",                     GSCAB_TT_NOTE,      GSCA_NT_C,                      0 },
    { "c#",                     GSCAB_TT_NOTE,      GSCA_NT_CSHARP,                 0 },
    { "d_",                     GSCAB_TT_NOTE,      GSCA_NT_D,                      0 },
    { "d#",                     GSCAB_TT_NOTE,      GSCA_NT_DSHARP,                 0 },
    { "e_",                     GSCAB_TT_NOTE,      GSCA_NT_E,                      0 },
    { "f_",                     GSCAB_TT_NOTE,      GSCA_NT_F,                      0 },
    { "f#",                     GSCAB_TT_NOTE,      GSCA_NT_FSHARP,                 0 },
    { "g_",                     GSCAB_TT_NOTE,      GSCA_NT_G,                      0 },
    { "g#",                     GSCAB_TT_NOTE,      GSCA_NT_GSHARP,                 0 },
    { "a_",                     GSCAB_TT_NOTE,      GSCA_NT_A,                      0 },
    { "a#",                     GSCAB_TT_NOTE,      GSCA_NT_ASHARP,                 0 },
    { "b_",                     GSCAB_TT_NOTE,      GSCA_NT_B,                      0 },
    { "db",                     GSCAB_TT_DATA,      GSCAB_DT_BYTE,                  1 },
    { "dw",                     GSCAB_TT_DATA,      GSCAB_DT_WORD,                  2 },
    { "dd",                     GSCAB_TT_DATA,      GSCAB_DT_DOUBLE_WORD,           4 },
    { "dq",                     GSCAB_TT_DATA,      GSCAB_DT_QUAD_WORD,             8 },
    { "channel_count",          GSCAB_TT_COMMAND,   GSCA_CHANNEL_COUNT_CMD,         0 },
    { "channel",                GSCAB_TT_COMMAND,   GSCA_CHANNEL_CMD,               9 },
    { "note",                   GSCAB_TT_COMMAND,   GSCA_NOTE_CMD,                  1 },
    { "drum_note",              GSCAB_TT_COMMAND,   GSCA_DRUM_NOTE_CMD,             1 },
    { "rest",                   GSCAB_TT_COMMAND,   GSCA_REST_CMD,                  1 },
    { "square_note",            GSCAB_TT_COMMAND,   GSCA_SQUARE_NOTE_CMD,           4 },
    { "noise_note",             GSCAB_TT_COMMAND,   GSCA_NOISE_NOTE_CMD,            3 },
    { "octave",                 GSCAB_TT_COMMAND,   GSCA_OCTAVE_CMD,                1 },
    { "note_type",              GSCAB_TT_COMMAND,   GSCA_NOTE_TYPE_CMD,             3 },
    { "drum_speed",             GSCAB_TT_COMMAND,   GSCA_DRUM_SPEED_CMD,            2 },
    { "transpose",              GSCAB_TT_COMMAND,   GSCA_TRANSPOSE_CMD,             2 },
    { "tempo",                  GSCAB_TT_COMMAND,   GSCA_TEMPO_CMD,                 3 },
    { "duty_cycle",             GSCAB_TT_COMMAND,   GSCA_DUTY_CYCLE_CMD,            2 },
    { "volume_envelope",        GSCAB_TT_COMMAND,   GSCA_VOLUME_ENVELOPE_CMD,       2 },
    { "pitch_sweep",            GSCAB_TT_COMMAND,   GSCA_PITCH_SWEEP_CMD,           2 },
    { "duty_cycle_pattern",     GSCAB_TT_COMMAND,   GSCA_DUTY_CYCLE_PATTERN_CMD,    2 },
    { "toggle_sfx",             GSCAB_TT_COMMAND,   GSCA_TOGGLE_SFX_CMD,            1 },
    { "pitch_slide",            GSCAB_TT_COMMAND,   GSCA_PITCH_SLIDE_CMD,           3 },
    { "vibrato",                GSCAB_TT_COMMAND,   GSCA_VIBRATO_CMD,               3 },
    { "toggle_noise",           GSCAB_TT_COMMAND,   GSCA_TOGGLE_NOISE_CMD,          2 },
    { "force_stereo_panning",   GSCAB_TT_COMMAND,   GSCA_FORCE_STEREO_PANNING_CMD,  2 },
    { "volume",                 GSCAB_TT_COMMAND,   GSCA_VOLUME_CMD,                2 },
    { "pitch_offset",           GSCAB_TT_COMMAND,   GSCA_PITCH_OFFSET_CMD,          3 },
    { "tempo_relative",         GSCAB_TT_COMMAND,   GSCA_TEMPO_RELATIVE_CMD,        2 },
    { "restart_channel",        GSCAB_TT_COMMAND,   GSCA_RESTART_CHANNEL_CMD,       9 },
    { "new_song",               GSCAB_TT_COMMAND,   GSCA_NEW_SONG_CMD,              3 },
    { "sfx_priority_on",        GSCAB_TT_COMMAND,   GSCA_SFX_PRIORITY_ON_CMD,       1 },
    { "sfx_priority_off",       GSCAB_TT_COMMAND,   GSCA_SFX_PRIORITY_OFF_CMD,      1 },
    { "stereo_panning",         GSCAB_TT_COMMAND,   GSCA_STEREO_PANNING_CMD,        2 },
    { "sfx_toggle_noise",       GSCAB_TT_COMMAND,   GSCA_SFX_TOGGLE_NOISE_CMD,      2 },
    { "set_condition",          GSCAB_TT_COMMAND,   GSCA_SET_CONDITION_CMD,         2 },
    { "sound_jump_if",          GSCAB_TT_COMMAND,   GSCA_SOUND_JUMP_IF_CMD,         10 },
    { "sound_jump",             GSCAB_TT_COMMAND,   GSCA_SOUND_JUMP_CMD,            9 },
    { "sound_loop",             GSCAB_TT_COMMAND,   GSCA_SOUND_LOOP_CMD,            10 },
    { "sound_call",             GSCAB_TT_COMMAND,   GSCA_SOUND_CALL_CMD,            9 },
    { "sound_ret",              GSCAB_TT_COMMAND,   GSCA_SOUND_RET_CMD,             1 },
    { "toggle_perfect_pitch",   GSCAB_TT_COMMAND,   GSCA_TOGGLE_PERFECT_PITCH_CMD,  0 },
    { "\0", 0, 0, 0 }
};

/* Public Functions ***********************************************************/

uint64_t gscabGetInteger (const gscabToken* token)
{
    gscaAssert(token);
    switch (token->type)
    {
        case GSCAB_TT_INTEGER:
            return strtoul(token->lexeme, nullptr, 10);
        case GSCAB_TT_HEXADECIMAL:
            return strtoul(token->lexeme, nullptr, 16);
        case GSCAB_TT_BOOLEAN:
        case GSCAB_TT_NOTE:
            return token->param;
        default:
            return 0;
    }
}

int64_t gscabGetSignedInteger (const gscabToken* token)
{
    return (int64_t) gscabGetInteger(token);
}

const gscabToken* gscabLookupKeyword (const char* lexeme)
{
    for (int i = 0; ; ++i)
    {
        const gscabToken* token = &KEYWORD_LOOKUP[i];
        if (token->lexeme[0] == '\0')
        {
            return nullptr;
        }

        if (strncmp(lexeme, token->lexeme, GSCAB_LEXEME_LEN) == 0)
        {
            return token;
        }
    }
}
