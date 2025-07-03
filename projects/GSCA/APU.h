/**
 * @file    GSCA/APU.h
 * @brief   An emulation of the Gameboy's audio processing unit (APU) hardware.
 */

#pragma once
#include <GSCA/Common.h>

/* Typedefs and Forward Declarations ******************************************/

typedef struct gscaAPU gscaAPU;

/* Enumerations ***************************************************************/

/**
 * @brief   Enumerates the Gameboy APU's four physical audio channels.
 */
typedef enum
{
    GSCA_AC_PULSE1,
    GSCA_AC_PULSE2,
    GSCA_AC_WAVE,
    GSCA_AC_NOISE
} gscaAudioChannel;

/**
 * @brief   Enumerates the duty cycle patterns of the waveforms which can be
 *          output by the pulse channels.
 */
typedef enum
{
    GSCA_DC_12_5,
    GSCA_DC_25,
    GSCA_DC_50,
    GSCA_DC_75
} gscaDutyCycle;

/**
 * @brief   Enumerates the sweep directions used by pulse channel 1's frequency
 *          sweep unit (controlled by register `NR10`).
 */
typedef enum
{
    GSCA_FSD_INCREASE,
    GSCA_FSD_DECREASE
} gscaFrequencySweepDirection;

/**
 * @brief   Enumerates the sweep directions used the pulse channels' and the
 *          noise channel's envelope sweep (fade) units.
 */
typedef enum
{
    GSCA_ESD_DECREASE,
    GSCA_ESD_INCREASE
} gscaEnvelopeSweepDirection;

/**
 * @brief   Enumerates the four levels of volume at which the wave channel
 *          outputs its sound.
 */
typedef enum
{
    GSCA_WOL_MUTE,
    GSCA_WOL_FULL,
    GSCA_WOL_HALF,
    GSCA_WOL_QUARTER
} gscaWaveOutputLevel;

/* Hardware Register Unions ***************************************************/

/**
 * @brief   Hardware register `NR52` contains the APU's master enable flag, as
 *          well as read-only flags which indicate which of the four audio
 *          channels are currently enabled.
 */
typedef union
{
    struct
    {
        uint8_t pulse1Enable    : 1;
        uint8_t pulse2Enable    : 1;
        uint8_t waveEnable      : 1;
        uint8_t noiseEnable     : 1;
        uint8_t                 : 3;
        uint8_t masterEnable    : 1;
    };
    uint8_t value;
} gscaMasterControl;

/**
 * @brief   Hardware register `NR51` controls the direction(s) in which each of
 *          the APU's audio channels' outputs are panned.
 */
typedef union
{
    struct
    {
        uint8_t pulse1Right : 1;
        uint8_t pulse2Right : 1;
        uint8_t waveRight   : 1;
        uint8_t noiseRight  : 1;
        uint8_t pulse1Left  : 1;
        uint8_t pulse2Left  : 1;
        uint8_t waveLeft    : 1;
        uint8_t noiseLeft   : 1;
    };
    uint8_t value;
} gscaSoundPanning;

/**
 * @brief   Hardware register `NR50` controls the APU's master volume, as well
 *          as panning settings for any external virtual sound hardware which
 *          may be used.
 */
typedef union
{
    struct
    {
        uint8_t rightVolume     : 3;
        uint8_t externalRight   : 1;
        uint8_t leftVolume      : 3;
        uint8_t externalLeft    : 1;
    };
    uint8_t value;
} gscaMasterVolume;

/**
 * @brief   Pulse channel 1 contains a frequency sweep unit which affects the
 *          pitch of its sound as it plays. Hardware register `NR10` controls
 *          this unit's settings.
 */
typedef union
{
    struct
    {
        uint8_t individualStep  : 3;
        uint8_t direction       : 1;
        uint8_t pace            : 3;
        uint8_t                 : 1;
    };
    uint8_t value;
} gscaFrequencySweep;

/**
 * @brief   Hardware registers `NR11`, `NR21` and `NR41` control the initial
 *          values of the length timer for pulse channel 1, pulse channel 2 and
 *          the noise channel, respectively. For the pulse channels, this
 *          register also contains settings for their wave duty cycle patterns.
 */
typedef union
{
    struct
    {
        uint8_t initialLengthTimer  : 6;
        uint8_t waveDuty            : 2;
    };
    uint8_t value;
} gscaLengthDuty;

/**
 * @brief   Hardware registers `NR12`, `NR22` and `NR42` control the volume and 
 *          envelope (fade) settings for pulse channel 1, pulse channel 2 and
 *          the noise channel, respectively.
 */
typedef union
{
    struct
    {
        uint8_t sweepPace           : 3;
        uint8_t envelopeDirection   : 1;
        uint8_t initialVolume       : 4;
    };
    uint8_t value;
} gscaVolumeEnvelope;

/**
 * @brief   Hardware registers `NR13`, `NR23` and `NR33` control the low 8 bits
 *          of the initial value of the pulse channels' and wave channel's
 *          11-bit period value.
 */
typedef union
{
    struct
    {
        uint8_t periodLow   : 8;
    };
    uint8_t value;
} gscaPeriodLow;

/**
 * @brief   Hardware registers `NR14`, `NR24`, `NR34` and `NR44` contain the
 *          trigger and length timer enable flags for the APU's four pulse
 *          channels. For the pulse and wave channels, these registers also
 *          contain the upper 3 bits of the initial value of their 11-bit period
 *          values.
 */
typedef union
{
    struct
    {
        uint8_t periodHigh      : 3;
        uint8_t                 : 3;
        uint8_t lengthEnable    : 1;
        uint8_t trigger         : 1;
    };
    uint8_t value;
} gscaChannelControl;

/**
 * @brief   Hardware register `NR30` is the wave channel's digital-to-analog
 *          converter (DAC) enable flag. The wave channel is the only channel
 *          in the APU whose DAC can be disabled externally via a hardware
 *          register.
 */
typedef union
{
    struct
    {
        uint8_t         : 7;
        uint8_t enable  : 1;
    };
    uint8_t value;
} gscaWaveDAC;

/**
 * @brief   Hardware register `NR31` controls the initial value of the wave
 *          channel's length timer. Unlike the other channels, whose length
 *          timers are 6 bits, the wave channel's length timer is a full 8 bits.
 */
typedef union
{
    struct
    {
        uint8_t initialLengthTimer  : 8;
    };
    uint8_t value;
} gscaWaveLengthTimer;

/**
 * @brief   Hardware register `NR32` controls the wave channel's volume output
 *          level. Unlike the other channels, the wave channel does not have
 *          a volume/envelope unit. Instead, it has a much more coarse volume
 *          control, consisting of only two bits.
 */
typedef union
{
    struct
    {
        uint8_t             : 5;
        uint8_t outputLevel : 2;
        uint8_t             : 1;
    };
    uint8_t value;
} gscaWaveOutput;

/**
 * @brief   Hardware register `NR43` controls the noise channel's frequency and
 *          randomness, which is driven by a linear-feedback shift register
 *          (LFSR).
 */
typedef union
{
    struct
    {
        uint8_t clockDivider    : 3;
        uint8_t lfsrWidth       : 1;
        uint8_t clockShift      : 4;
    };
    uint8_t value;
} gscaNoiseFrequency;

/* Audio Sample Structure *****************************************************/

/**
 * @brief   Contains a floating-point sample output from the APU.
 */
typedef struct
{
    float left, right;
} gscaAudioSample;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Creates a new GSCA APU emulation context.
 * 
 * @return  A pointer to the new emulation context.
 */
GSCA_API gscaAPU* gscaCreateAPU ();

/**
 * @brief   Resets the given GSCA APU emulation context, zeroing out its
 *          physical channels and resetting its hardware registers to their
 *          initial values.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 */
GSCA_API void gscaResetAPU (gscaAPU* apu);

/**
 * @brief   Destroys the given GSCA APU emulation context, freeing its memory.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 */
GSCA_API void gscaDestroyAPU (gscaAPU* apu);

/**
 * @brief   Retrieves the current state of the APU's current audio sample.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A pointer to the current audio sample.
 */
GSCA_API const gscaAudioSample* gscaGetCurrentSample (const gscaAPU* apu);

/**
 * @brief   Sets the wave pattern used by the APU's wave channel.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   pattern A 32-character hexadecimal string containing the new wave pattern.
 * 
 * @return  `true` if the given pattern is valid; `false` if not.
 */
GSCA_API bool gscaSetWavePattern (gscaAPU* apu, const char* pattern);

/**
 * @brief   Ticks the APU, updating its internal state and channels.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  `true` if the APU's audio sample was updated, `false` if not.
 */
GSCA_API bool gscaTickAPU (gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR52`, which contains the
 *          APU's master enable and channel enable flags.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR52 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR51`, which contains the
 *          APU's channel output panning settings.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR51 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR50`, which contains the
 *          APU's master volume and external VIN output panning settings.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR50 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR10`, which contains
 *          settings controlling pulse channel 1's frequency sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR10 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR11`, which contains
 *          settings controlling pulse channel 1's length timer and duty cycle
 *          pattern.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR11 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR12`, which controls
 *          pulse channel 1's volume and envelope sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR12 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR13`, which contains
 *          the lower 8 bits of pulse channel 1's 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR13 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR14`, which contains
 *          pulse channel 1's trigger bit, length enable flag and the upper 3
 *          bits of its 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR14 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR21`, which contains
 *          settings controlling pulse channel 2's length timer and duty cycle
 *          pattern.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR21 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR22`, which controls
 *          pulse channel 2's volume and envelope sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR22 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR23`, which contains
 *          the lower 8 bits of pulse channel 2's 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR23 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR24`, which contains
 *          pulse channel 2's trigger bit, length enable flag and the upper 3
 *          bits of its 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR24 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR30`, which contains the
 *          wave channel's digital-to-analog converter's (DAC's) enable flag.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR30 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR31`, which contains the
 *          initial value of the wave channel's length timer.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR31 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR32`, which contains the
 *          wave channel's volume output level setting.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR32 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR33`, which contains
 *          the lower 8 bits of the wave channel's 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR33 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR34`, which contains the
 *          wave channel's trigger bit, length enable flag and the upper 3
 *          bits of its 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR34 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR41`, which contains the
 *          initial value of the noise channel's length timer.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR41 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR42`, which controls the
 *          noise channel's volume and envelope sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR42 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR43`, which controls the
 *          noise channel's linear-feedback shift register (LFSR).
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR43 (const gscaAPU* apu);

/**
 * @brief   Reads the value of hardware register `NR44`, which contains the
 *          noise channel's trigger bit and length timer enable flag.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * 
 * @return  A byte value containing the readable bits of the hardware register's
 *          value.
 */
GSCA_API uint8_t gscaReadNR44 (const gscaAPU* apu);

/**
 * @brief   Sets the value of hardware register `NR52`, which contains the
 *          APU's master enable and channel enable flags.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR52 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR51`, which contains the
 *          APU's channel output panning settings.
 * 
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR51 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR50`, which contains the
 *          APU's master volume and external VIN output panning settings.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR50 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR10`, which contains
 *          settings controlling pulse channel 1's frequency sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR10 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR11`, which contains
 *          settings controlling pulse channel 1's length timer and duty cycle
 *          pattern.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR11 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR12`, which controls
 *          pulse channel 1's volume and envelope sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR12 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR13`, which contains
 *          the lower 8 bits of pulse channel 1's 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR13 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR14`, which contains
 *          pulse channel 1's trigger bit, length enable flag and the upper 3
 *          bits of its 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR14 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR21`, which contains
 *          settings controlling pulse channel 2's length timer and duty cycle
 *          pattern.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR21 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR22`, which controls
 *          pulse channel 2's volume and envelope sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR22 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR23`, which contains
 *          the lower 8 bits of pulse channel 2's 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR23 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR24`, which contains
 *          pulse channel 2's trigger bit, length enable flag and the upper 3
 *          bits of its 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR24 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR30`, which contains the
 *          wave channel's digital-to-analog converter's (DAC's) enable flag.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR30 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR31`, which contains the
 *          initial value of the wave channel's length timer.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR31 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR32`, which contains the
 *          wave channel's volume output level setting.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR32 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR33`, which contains
 *          the lower 8 bits of the wave channel's 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR33 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR34`, which contains the
 *          wave channel's trigger bit, length enable flag and the upper 3
 *          bits of its 11-bit initial period value.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR34 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR41`, which contains the
 *          initial value of the noise channel's length timer.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR41 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR42`, which controls the
 *          noise channel's volume and envelope sweep unit.
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR42 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR43`, which controls the
 *          noise channel's linear-feedback shift register (LFSR).
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR43 (gscaAPU* apu, uint8_t value);

/**
 * @brief   Sets the value of hardware register `NR44`, which contains the
 *          noise channel's trigger bit and length timer enable flag.
 * 
 * 
 * @param   apu     A pointer to the GSCA APU emulation context.
 * @param   value   The hardware register's new value.
 */
GSCA_API void gscaWriteNR44 (gscaAPU* apu, uint8_t value);
