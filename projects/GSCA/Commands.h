/**
 * @file    GSCA/Commands.h
 */

#pragma once

#define GSCA_CHANNEL_COUNT_CMD          0xC0
#define GSCA_CHANNEL_CMD                0xC1
#define GSCA_NOTE_CMD                   0xC2
#define GSCA_DRUM_NOTE_CMD              0xC3
#define GSCA_REST_CMD                   0xC4
#define GSCA_SQUARE_NOTE_CMD            0xC5
#define GSCA_NOISE_NOTE_CMD             0xC6
#define GSCA_TOGGLE_PERFECT_PITCH_CMD   0xC8
#define GSCA_OCTAVE_CMD                 0xD0
#define GSCA_OCTAVE_CMD0                0xD0
#define GSCA_OCTAVE_CMD1                0xD1
#define GSCA_OCTAVE_CMD2                0xD2
#define GSCA_OCTAVE_CMD3                0xD3
#define GSCA_OCTAVE_CMD4                0xD4
#define GSCA_OCTAVE_CMD5                0xD5
#define GSCA_OCTAVE_CMD6                0xD6
#define GSCA_OCTAVE_CMD7                0xD7
#define GSCA_NOTE_TYPE_CMD              0xD8
#define GSCA_DRUM_SPEED_CMD             0xC7
#define GSCA_TRANSPOSE_CMD              0xD9
#define GSCA_TEMPO_CMD                  0xDA
#define GSCA_DUTY_CYCLE_CMD             0xDB
#define GSCA_VOLUME_ENVELOPE_CMD        0xDC
#define GSCA_PITCH_SWEEP_CMD            0xDD
#define GSCA_DUTY_CYCLE_PATTERN_CMD     0xDE
#define GSCA_TOGGLE_SFX_CMD             0XDF
#define GSCA_PITCH_SLIDE_CMD            0XE0
#define GSCA_VIBRATO_CMD                0XE1
#define GSCA_TOGGLE_NOISE_CMD           0XE3
#define GSCA_FORCE_STEREO_PANNING_CMD   0XE4
#define GSCA_VOLUME_CMD                 0XE5
#define GSCA_PITCH_OFFSET_CMD           0XE6
#define GSCA_TEMPO_RELATIVE_CMD         0XE9
#define GSCA_RESTART_CHANNEL_CMD        0XEA
#define GSCA_NEW_SONG_CMD               0XEB
#define GSCA_SFX_PRIORITY_ON_CMD        0XEC
#define GSCA_SFX_PRIORITY_OFF_CMD       0XED
#define GSCA_STEREO_PANNING_CMD         0XEF
#define GSCA_SFX_TOGGLE_NOISE_CMD       0XF0
#define GSCA_SET_CONDITION_CMD          0XFA
#define GSCA_SOUND_JUMP_IF_CMD          0XFB
#define GSCA_SOUND_JUMP_CMD             0XFC
#define GSCA_SOUND_LOOP_CMD             0XFD
#define GSCA_SOUND_CALL_CMD             0XFE
#define GSCA_SOUND_RET_CMD              0XFF
#define GSCA_FIRST_MUSIC_CMD            GSCA_OCTAVE_CMD

#define GSCA_SQUARE_NOTE(length, volume, fade, frequency) \
    length, \
    (volume << 4) | (fade < 0 ? (0b1000 | (fade * -1)) : fade), \
    frequency

#define GSCA_NOISE_NOTE(length, volume, fade, frequency) \
    length, \
    (volume << 4) | (fade < 0 ? (0b1000 | (fade * -1)) : fade), \
    frequency

#define GSCA_SOUND_RET GSCA_SOUND_RET_CMD
