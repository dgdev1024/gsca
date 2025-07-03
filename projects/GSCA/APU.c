/**
 * @file    GSCA/APU.c
 */

#include <GSCA/APU.h>

/* Audio Channel Structures ***************************************************/

typedef struct
{
    float       dacOutput;
    uint16_t    period;
    uint16_t    periodDivider;
    uint8_t     lengthTimer;
    uint8_t     volume;
    uint8_t     wavePointer;
    uint8_t     frequencyTicks;
    uint8_t     envelopeTicks;
    uint8_t     dacInput;
    bool        dacEnable;
} gscaPulseChannel;

typedef struct
{
    float       dacOutput;
    uint16_t    period;
    uint16_t    periodDivider;
    uint8_t     lengthTimer;
    uint8_t     sampleIndex;
    uint8_t     dacInput;
} gscaWaveChannel;

typedef struct
{
    float       dacOutput;
    uint32_t    clockFrequency;
    uint16_t    lfsr;
    uint8_t     lengthTimer;
    uint8_t     volume;
    uint8_t     envelopeTicks;
    uint8_t     dacInput;
    bool        dacEnable;
} gscaNoiseChannel;

/* APU Structure **************************************************************/

typedef struct gscaAPU
{
    uint8_t             waveMemory[GSCA_WAVE_RAM_NIBBLE_SIZE];
    gscaPulseChannel    pulse1;
    gscaPulseChannel    pulse2;
    gscaWaveChannel     wave;
    gscaNoiseChannel    noise;
    gscaAudioSample     currentSample;
    gscaAudioSample     previousInput;
    gscaAudioSample     previousOutput;
    uint32_t            ticks;
    uint16_t            divider;
    uint16_t            clockFrequency;
    gscaMasterControl   nr52;
    gscaSoundPanning    nr51;
    gscaMasterVolume    nr50;
    gscaFrequencySweep  nr10;
    gscaLengthDuty      nr11;
    gscaVolumeEnvelope  nr12;
    gscaPeriodLow       nr13;
    gscaChannelControl  nr14;
    gscaLengthDuty      nr21;
    gscaVolumeEnvelope  nr22;
    gscaPeriodLow       nr23;
    gscaChannelControl  nr24;
    gscaWaveDAC         nr30;
    gscaWaveLengthTimer nr31;
    gscaWaveOutput      nr32;
    gscaPeriodLow       nr33;
    gscaChannelControl  nr34;
    gscaLengthDuty      nr41;
    gscaVolumeEnvelope  nr42;
    gscaNoiseFrequency  nr43;
    gscaChannelControl  nr44;
} gscaAPU;

/* Static Function Prototypes *************************************************/

static void gscaTriggerChannel (gscaAPU*, gscaEnum);
static void gscaResetPeriod (gscaAPU*, gscaEnum);
static void gscaTickPulseChannels (gscaAPU*);
static void gscaTickWaveChannel (gscaAPU*);
static void gscaTickNoiseChannel (gscaAPU*);
static void gscaTickLengthTimers (gscaAPU*);
static void gscaTickFrequencySweep (gscaAPU*);
static void gscaTickEnvelopeSweeps (gscaAPU*);
static void gscaUpdateNoiseClockFrequency (gscaAPU*);
static void gscaUpdateAudioSample (gscaAPU*);

/* Static Functions ***********************************************************/

void gscaTriggerChannel (gscaAPU* apu, gscaEnum type)
{
    gscaAssert(apu);
    switch (type)
    {
        case GSCA_AC_PULSE1:
        {
            gscaPulseChannel* chan = &apu->pulse1;
            chan->lengthTimer = apu->nr11.initialLengthTimer;
            chan->volume = apu->nr12.initialVolume;
            chan->period = (apu->nr14.periodHigh << 8) | apu->nr13.periodLow;
            chan->periodDivider = chan->period;
            chan->wavePointer = 0;
            chan->frequencyTicks = 0;
            chan->envelopeTicks = 0;
            apu->nr52.pulse1Enable = chan->dacEnable;
        } break;
        case GSCA_AC_PULSE2:
        {
            gscaPulseChannel* chan = &apu->pulse2;
            chan->lengthTimer = apu->nr21.initialLengthTimer;
            chan->volume = apu->nr22.initialVolume;
            chan->period = (apu->nr24.periodHigh << 8) | apu->nr23.periodLow;
            chan->periodDivider = chan->period;
            chan->wavePointer = 0;
            chan->frequencyTicks = 0;
            chan->envelopeTicks = 0;
            apu->nr52.pulse2Enable = chan->dacEnable;
        } break;
        case GSCA_AC_WAVE:
        {
            gscaWaveChannel* chan = &apu->wave;
            chan->lengthTimer = apu->nr31.initialLengthTimer;
            chan->period = (apu->nr34.periodHigh << 8) | apu->nr33.periodLow;
            chan->periodDivider = chan->period;
            chan->sampleIndex = 0;
            apu->nr52.waveEnable = apu->nr30.enable;
        } break;
        case GSCA_AC_NOISE:
        {
            gscaNoiseChannel* chan = &apu->noise;
            chan->lengthTimer = apu->nr41.initialLengthTimer;
            chan->volume = apu->nr42.initialVolume;
            chan->lfsr = 0;
            chan->envelopeTicks = 0;
            apu->nr52.noiseEnable = chan->dacEnable;
        } break;
    }
}

static void gscaResetPeriod (gscaAPU* apu, gscaEnum type)
{
    switch (type)
    {
        case GSCA_AC_PULSE1:
            apu->pulse1.period =
                (apu->nr13.periodLow | (apu->nr14.periodHigh << 8));
            apu->pulse1.periodDivider = apu->pulse1.period;
            break;
        case GSCA_AC_PULSE2:
            apu->pulse2.period =
                (apu->nr23.periodLow | (apu->nr24.periodHigh << 8));
            apu->pulse2.periodDivider = apu->pulse2.period;
            break;
        case GSCA_AC_WAVE:
            apu->wave.period =
                (apu->nr33.periodLow | (apu->nr34.periodHigh << 8));
            apu->wave.periodDivider = apu->wave.period;
            break;
        default: break;
    }
}

void gscaTickPulseChannels (gscaAPU* apu)
{
    static const uint8_t WAVE_DUTY_PATTERNS[] = {
        0b00000001, 0b00000011, 0b00001111, 0b00111111
    };

    gscaPulseChannel* chan = &apu->pulse1;
    if (apu->nr52.pulse1Enable && ++chan->periodDivider > 0x800)
    {
        chan->periodDivider = chan->period;
        chan->wavePointer = (chan->wavePointer + 1) & 0b111;
        chan->dacInput =
            (WAVE_DUTY_PATTERNS[apu->nr11.waveDuty] >> chan->wavePointer) & 0b1;
        chan->dacInput *= chan->volume;
        chan->dacOutput = -(((float) chan->dacInput / 7.5f) - 1.0f);
    }

    chan = &apu->pulse2;
    if (apu->nr52.pulse2Enable && ++chan->periodDivider > 0x800)
    {
        chan->periodDivider = chan->period;
        chan->wavePointer = (chan->wavePointer + 1) & 0b111;
        chan->dacInput =
            (WAVE_DUTY_PATTERNS[apu->nr21.waveDuty] >> chan->wavePointer) & 0b1;
        chan->dacInput *= chan->volume;
        chan->dacOutput = -(((float) chan->dacInput / 7.5f) - 1.0f);
    }
}

void gscaTickWaveChannel (gscaAPU* apu)
{
    gscaWaveChannel* chan = &apu->wave;
    if (apu->nr52.waveEnable && ++chan->periodDivider > 0x800)
    {
        chan->periodDivider = chan->period;
        chan->sampleIndex = (chan->sampleIndex + 1) & 0b11111;
        chan->dacInput = apu->waveMemory[chan->sampleIndex];
        switch (apu->nr32.outputLevel)
        {
            case GSCA_WOL_MUTE:     chan->dacInput = 0; break;
            case GSCA_WOL_FULL:     break;
            case GSCA_WOL_HALF:     chan->dacInput >>= 1; break;
            case GSCA_WOL_QUARTER:  chan->dacInput >>= 2; break;
        }
        chan->dacOutput = -(((float) chan->dacInput / 7.5f) - 1.0f);
    }
}

void gscaTickNoiseChannel (gscaAPU* apu)
{
    gscaNoiseChannel* chan = &apu->noise;
    if (apu->nr52.noiseEnable)
    {
        uint8_t bit = ((chan->lfsr & 0b1) == ((chan->lfsr >> 1) & 0b1));

        chan->lfsr |= (bit << 15);
        if (apu->nr43.lfsrWidth) { chan->lfsr |= (bit << 7); }

        chan->lfsr >>= 1;
        chan->lfsr &= ~(1 << 15);
        if (apu->nr43.lfsrWidth) { chan->lfsr &= ~(1 << 7); }

        chan->dacInput = bit * chan->volume;
        chan->dacOutput = -(((float) chan->dacInput / 7.5f) - 1.0f);
    }
}

void gscaTickLengthTimers (gscaAPU* apu)
{
    gscaPulseChannel* pchan = &apu->pulse1;
    if (apu->nr52.pulse1Enable && apu->nr14.lengthEnable)
    {
        if (++pchan->lengthTimer >= 0b111111)
            apu->nr52.pulse1Enable = false;
    }

    pchan = &apu->pulse2;
    if (apu->nr52.pulse2Enable && apu->nr24.lengthEnable)
    {
        if (++pchan->lengthTimer >= 0b111111)
            apu->nr52.pulse2Enable = false;
    }

    gscaWaveChannel* wchan = &apu->wave;
    if (apu->nr52.waveEnable && apu->nr34.lengthEnable)
    {
        if (++wchan->lengthTimer == 0b11111111)
            apu->nr52.waveEnable = false;
    }

    gscaNoiseChannel* nchan = &apu->noise;
    if (apu->nr52.noiseEnable && apu->nr44.lengthEnable)
    {
        if (++nchan->lengthTimer >= 0b111111)
            apu->nr52.noiseEnable = false;
    }
}

void gscaTickFrequencySweep (gscaAPU* apu)
{
    gscaPulseChannel* chan = &apu->pulse1;

    if (apu->nr52.pulse1Enable && apu->nr10.individualStep)
    {
        uint16_t periodDelta = (chan->period >> apu->nr10.individualStep);
        if (
            apu->nr10.direction == GSCA_FSD_INCREASE && 
            chan->period + periodDelta > 0x7FF
        )
        {
            apu->nr52.pulse1Enable = false;
            return;
        }

        if (++chan->frequencyTicks >= apu->nr10.pace)
        {
            chan->frequencyTicks = 0;
            if (apu->nr10.direction == GSCA_FSD_INCREASE)
                { chan->period += periodDelta; }
            else
                { chan->period -= periodDelta; }

            chan->periodDivider = chan->period;
        }
    }
}

void gscaTickEnvelopeSweeps (gscaAPU* apu)
{
    gscaPulseChannel* pchan = &apu->pulse1;
    if (apu->nr52.pulse1Enable && apu->nr12.sweepPace > 0)
    {
        if (++pchan->envelopeTicks >= apu->nr12.sweepPace)
        {
            if (
                apu->nr12.envelopeDirection == GSCA_ESD_INCREASE &&
                pchan->volume < 0xF
            )
            {
                pchan->volume++;
            }
            else if (
                apu->nr12.envelopeDirection == GSCA_ESD_DECREASE &&
                pchan->volume > 0x0
            )
            {
                pchan->volume--;
            }

            pchan->envelopeTicks = 0;
        }
    }

    pchan = &apu->pulse2;
    if (apu->nr52.pulse2Enable && apu->nr22.sweepPace > 0)
    {
        if (++pchan->envelopeTicks >= apu->nr22.sweepPace)
        {
            if (
                apu->nr22.envelopeDirection == GSCA_ESD_INCREASE &&
                pchan->volume < 0xF
            )
            {
                pchan->volume++;
            }
            else if (
                apu->nr22.envelopeDirection == GSCA_ESD_DECREASE &&
                pchan->volume > 0x0
            )
            {
                pchan->volume--;
            }

            pchan->envelopeTicks = 0;
        }
    }

    gscaNoiseChannel* nchan = &apu->noise;
    if (apu->nr52.noiseEnable && apu->nr42.sweepPace > 0)
    {
        if (++nchan->envelopeTicks >= apu->nr42.sweepPace)
        {
            if (
                apu->nr42.envelopeDirection == GSCA_ESD_INCREASE &&
                nchan->volume < 0xF
            )
            {
                nchan->volume++;
            }
            else if (
                apu->nr42.envelopeDirection == GSCA_ESD_DECREASE &&
                nchan->volume > 0x0
            )
            {
                nchan->volume--;
            }

            nchan->envelopeTicks = 0;
        }
    }
}

void gscaUpdateNoiseClockFrequency (gscaAPU* apu)
{
    gscaNoiseChannel* chan = &apu->noise;
    if (apu->nr43.clockDivider == 0)
    {
        chan->clockFrequency = 262144 / (0.5 * pow(2, apu->nr43.clockShift));
    }
    else
    {
        chan->clockFrequency =
            262144 / (apu->nr43.clockDivider * pow(2, apu->nr43.clockShift));
    }

    chan->clockFrequency = (4194304.0f / chan->clockFrequency);
}

void gscaUpdateAudioSample (gscaAPU* apu)
{
    static const float HPF_ALPHA = 0.999958f;

    gscaAudioSample* sample = &apu->currentSample;
    gscaAudioSample* in = &apu->previousInput;
    gscaAudioSample* out = &apu->previousOutput;

    sample->left = 0.0f;
    sample->right = 0.0f;

    if (apu->nr52.pulse1Enable && apu->pulse1.dacEnable)
    {
        if (apu->nr51.pulse1Left)
            sample->left += apu->pulse1.dacOutput;
        if (apu->nr51.pulse1Right)
            sample->right += apu->pulse1.dacOutput;
    }

    if (apu->nr52.pulse2Enable && apu->pulse2.dacEnable)
    {
        if (apu->nr51.pulse2Left)
            sample->left += apu->pulse2.dacOutput;
        if (apu->nr51.pulse2Right)
            sample->right += apu->pulse2.dacOutput;
    }

    if (apu->nr52.waveEnable && apu->nr30.enable)
    {
        if (apu->nr51.waveLeft)
            sample->left += apu->wave.dacOutput;
        if (apu->nr51.waveRight)
            sample->right += apu->wave.dacOutput;
    }

    if (apu->nr52.noiseEnable && apu->noise.dacEnable)
    {
        if (apu->nr51.noiseLeft)
            sample->left += apu->noise.dacOutput;
        if (apu->nr51.noiseRight)
            sample->right += apu->noise.dacOutput;
    }

    sample->left  *= (0.125f * (apu->nr50.leftVolume  + 1));
    sample->right *= (0.125f * (apu->nr50.rightVolume + 1));

    float newLeft  = sample->left  - in->left  + HPF_ALPHA * out->left;
    float newRight = sample->right - in->right + HPF_ALPHA * out->right;
    in->left = sample->left; in->right = sample->right;
    sample->left = newLeft; sample->right = newRight;
    out->left = newLeft; out->right = newRight;
    sample->left /= 4.0f; sample->right /= 4.0f;
}

/* Public Functions ***********************************************************/

gscaAPU* gscaCreateAPU ()
{
    gscaAPU* apu = gscaCreateZero(1, gscaAPU);
    gscaExpectp(apu, "Cannot create APU context");
    gscaResetAPU(apu);
    return apu;
}

void gscaResetAPU (gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    gscaZero(apu, 1, gscaAPU);
    apu->nr52.value = 0xF1;
    apu->nr51.value = 0xF3;
    apu->nr50.value = 0x77;
    apu->nr10.value = 0x80;
    apu->nr11.value = 0xBF;
    apu->nr12.value = 0xF3;
    apu->nr13.value = 0xFF;
    apu->nr14.value = 0xBF;
    apu->nr21.value = 0x3F;
    apu->nr22.value = 0x00;
    apu->nr23.value = 0xFF;
    apu->nr24.value = 0xBF;
    apu->nr30.value = 0x7F;
    apu->nr31.value = 0xFF;
    apu->nr32.value = 0x9F;
    apu->nr33.value = 0xFF;
    apu->nr34.value = 0xBF;
    apu->nr41.value = 0xFF;
    apu->nr42.value = 0x00;
    apu->nr43.value = 0x00;
    apu->nr44.value = 0xBF;
    apu->clockFrequency = 4194304 / GSCA_DEFAULT_SAMPLE_RATE;
    gscaUpdateNoiseClockFrequency(apu);
}

void gscaDestroyAPU (gscaAPU* apu)
{
    if (apu != NULL)
    {
        gscaDestroy(apu);
    }
}

const gscaAudioSample* gscaGetCurrentSample (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return &apu->currentSample;
}

bool gscaSetWavePattern (gscaAPU* apu, const char* pattern)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    gscaExpect(pattern, "Pointer 'pattern' is NULL.\n");

    if (strlen(pattern) != GSCA_WAVE_RAM_NIBBLE_SIZE)
    {
        gscaErr("Wave pattern string must be 32 characters in length.\n");
        return false;
    }

    for (int i = 0; i < GSCA_WAVE_RAM_NIBBLE_SIZE; ++i)
    {
        char c = pattern[i];
        if (!isxdigit(c))
        {
            gscaErr("Wave pattern character %d ('%c') is not hexadecimal.\n",
                i, c);
            return false;
        }

        apu->waveMemory[i] = (strtoul(&c, NULL, 16) & 0xF);
    }

    return true;
}

bool gscaTickAPU (gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");

    bool bitWasHigh = (gscaCheckBit((apu->ticks++), 12) == 1);
    bool bitNowLow  = (gscaCheckBit( apu->ticks,    12) == 0);

    if (apu->ticks % 2 == 0)
        { gscaTickWaveChannel(apu); }
    if (apu->ticks % 4 == 0)
        { gscaTickPulseChannels(apu); }
    if (apu->ticks % apu->noise.clockFrequency == 0)
        { gscaTickNoiseChannel(apu); }

    if (bitWasHigh && bitNowLow)
    {
        apu->divider++;
        if (apu->divider % 2 == 0)
            { gscaTickLengthTimers(apu); }
        if (apu->divider % 4 == 0)
            { gscaTickFrequencySweep(apu); }
        if (apu->divider % 8 == 0)
            { gscaTickEnvelopeSweeps(apu); }
    }

    if (apu->ticks % apu->clockFrequency == 0)
        { gscaUpdateAudioSample(apu); return true; }

    return false;
}

uint8_t gscaReadNR52 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr52.value;
}

uint8_t gscaReadNR51 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr51.value;
}

uint8_t gscaReadNR50 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr50.value;
}

uint8_t gscaReadNR10 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr10.value;
}

uint8_t gscaReadNR11 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr11.value & 0b11000000;
}

uint8_t gscaReadNR12 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr12.value;
}

uint8_t gscaReadNR14 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr14.value & 0b01111000;
}

uint8_t gscaReadNR21 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr21.value & 0b11000000;
}

uint8_t gscaReadNR22 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr22.value;
}

uint8_t gscaReadNR24 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr24.value & 0b01111000;
}

uint8_t gscaReadNR30 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr30.value;
}

uint8_t gscaReadNR32 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr32.value;
}

uint8_t gscaReadNR34 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr34.value & 0b01111000;
}

uint8_t gscaReadNR42 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr42.value;
}

uint8_t gscaReadNR43 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr43.value;
}

uint8_t gscaReadNR44 (const gscaAPU* apu)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    return apu->nr44.value & 0b01111111;
}

void gscaWriteNR52 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    apu->nr52.value |= (value & 0b11110000);
    if (apu->nr52.masterEnable == false)
    {
        apu->nr52.value = 0x00;
        apu->nr51.value = 0x00;
        apu->nr50.value = 0x00;
        apu->nr10.value = 0x00;
        apu->nr11.value = 0x00;
        apu->nr12.value = 0x00;
        apu->nr13.value = 0x00;
        apu->nr14.value = 0x00;
        apu->nr21.value = 0x00;
        apu->nr22.value = 0x00;
        apu->nr23.value = 0x00;
        apu->nr24.value = 0x00;
        apu->nr30.value = 0x00;
        apu->nr31.value = 0x00;
        apu->nr32.value = 0x00;
        apu->nr33.value = 0x00;
        apu->nr34.value = 0x00;
        apu->nr41.value = 0x00;
        apu->nr42.value = 0x00;
        apu->nr43.value = 0x00;
        apu->nr44.value = 0x00;
    }
}

void gscaWriteNR51 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr51.value = value;
    }
}

void gscaWriteNR50 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr50.value = value;
    }
}

void gscaWriteNR10 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr10.value = value;
    }
}

void gscaWriteNR11 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr11.value = value;
    }
}

void gscaWriteNR12 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr12.value = value;
        if (
            apu->nr12.envelopeDirection == GSCA_ESD_DECREASE &&
            apu->nr12.initialVolume == 0
        )
        {
            apu->pulse1.dacEnable = false;
            apu->nr52.pulse1Enable = false;
        }
        else
        {
            apu->pulse1.dacEnable = true;
        }
    }
}

void gscaWriteNR13 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr13.value = value;
        gscaResetPeriod(apu, GSCA_AC_PULSE1);
    }
}

void gscaWriteNR14 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr14.value = value;
        gscaResetPeriod(apu, GSCA_AC_PULSE1);
        if (apu->nr14.trigger == true)
        {
            gscaTriggerChannel(apu, GSCA_AC_PULSE1);
        }
    }
}

void gscaWriteNR21 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr21.value = value;
    }
}

void gscaWriteNR22 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr22.value = value;
        if (
            apu->nr22.envelopeDirection == GSCA_ESD_DECREASE &&
            apu->nr22.initialVolume == 0
        )
        {
            apu->pulse2.dacEnable = false;
            apu->nr52.pulse2Enable = false;
        }
        else
        {
            apu->pulse2.dacEnable = true;
        }
    }
}

void gscaWriteNR23 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr23.value = value;
        gscaResetPeriod(apu, GSCA_AC_PULSE2);
    }
}

void gscaWriteNR24 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr24.value = value;
        gscaResetPeriod(apu, GSCA_AC_PULSE2);
        if (apu->nr24.trigger == true)
        {
            gscaTriggerChannel(apu, GSCA_AC_PULSE2);
        }
    }
}

void gscaWriteNR30 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr30.value = value;
        if (apu->nr30.enable == false)
        {
            apu->nr52.waveEnable = false;
        }
    }
}

void gscaWriteNR31 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr31.value = value;
    }
}

void gscaWriteNR32 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr32.value = value;
    }
}

void gscaWriteNR33 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr33.value = value;
        gscaResetPeriod(apu, GSCA_AC_WAVE);
    }
}

void gscaWriteNR34 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr34.value = value;
        gscaResetPeriod(apu, GSCA_AC_WAVE);
        if (apu->nr34.trigger == true)
        {
            gscaTriggerChannel(apu, GSCA_AC_WAVE);
        }
    }
}

void gscaWriteNR41 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr41.value = value;
    }
}

void gscaWriteNR42 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr42.value = value;
        if (
            apu->nr42.envelopeDirection == GSCA_ESD_DECREASE &&
            apu->nr42.initialVolume == 0
        )
        {
            apu->noise.dacEnable = false;
            apu->nr52.noiseEnable = false;
        }
        else
        {
            apu->noise.dacEnable = true;
        }
    }
}

void gscaWriteNR43 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr43.value = value;
        gscaUpdateNoiseClockFrequency(apu);
    }
}

void gscaWriteNR44 (gscaAPU* apu, uint8_t value)
{
    gscaExpect(apu, "Pointer 'apu' is NULL.\n");
    if (apu->nr52.masterEnable == true)
    {
        apu->nr44.value = value;
        if (apu->nr44.trigger == true)
        {
            gscaTriggerChannel(apu, GSCA_AC_NOISE);
        }
    }
}
