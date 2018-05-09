#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <portaudio.h>
#include <stdlib.h>

#include "../oscillator/osc.h"
#include "../utils/util.h"
#include "../audio/audio.h"
#include "../params.h"
#include "control.h"

static audioData        audio_data;
static ControlStatus    *control_status;

Sample calculateSample(float time) {
    Sample nextSample;

    float sawVal    = 0;
    float sinVal    = 0;
    float sqrVal    = 0;
    float triVal    = 0;

    sawVal          = oscillate_saw(time, control_status->frequency, control_status->phase[0] / (VALUE_RANGE / 2));
    sawVal          = sawVal * control_status->ampWave[0] / VALUE_RANGE;

    sinVal          = oscillate_sin(time, control_status->frequency, control_status->phase[1] / (VALUE_RANGE / 2));
    sinVal          = sinVal * control_status->ampWave[1] / VALUE_RANGE;

    sqrVal          = oscillate_sqr(time, control_status->frequency, control_status->phase[2] / (VALUE_RANGE / 2));
    sqrVal          = sqrVal * control_status->ampWave[2] / VALUE_RANGE;

    triVal          = oscillate_tri(time, control_status->frequency, control_status->phase[3] / (VALUE_RANGE / 2));
    triVal          = triVal * control_status->ampWave[3] / VALUE_RANGE;

    nextSample.left_data    =  sawVal + sinVal + sqrVal + triVal;
    nextSample.right_data   =  sawVal + sinVal + sqrVal + triVal;

    return nextSample;
}

void initEngine(ControlStatus *status) {
    startSignalHandler();
    Sample nextSample;

    control_status  = status;

    float notes[127];
    float baseFreq  = 440; // a is 440 hz...
    float noteDiv   = 0;
    for (int freqInd = 21; freqInd < 109; freqInd++)
    {
        noteDiv = (float) (freqInd - 69) / (float) 12;
        notes[freqInd] = baseFreq * powf(2.0, noteDiv);
    }

    // init frames with 0
    for (unsigned int frameInd = 0; frameInd < FRAME_SIZE; frameInd++) {
        for (unsigned int bufIndex = 0; bufIndex < BUFFER_SIZE; bufIndex++) {
            audio_data.left_data[frameInd][bufIndex]    = 0;
            audio_data.right_data[frameInd][bufIndex]   = 0;
        }
    }
    initAudioStream(&audio_data);

    control_status->frequency    = notes[57];

    unsigned int lastFrame  = 0;
    unsigned int calSamples = 0;

    while (control_status->engineRunning == 1) {
        while (lastFrame != audio_data.lastFrame) {
            for (unsigned int bufIndex = 0; bufIndex < BUFFER_SIZE; bufIndex++) {
                nextSample  = calculateSample((float) calSamples / (float) SAMPLE_RATE);
                audio_data.left_data[lastFrame][bufIndex]       = nextSample.left_data;
                audio_data.right_data[lastFrame][bufIndex]      = nextSample.right_data;
                calSamples  = (calSamples + 1) % SAMPLE_RATE;
            }
            lastFrame   = (lastFrame + 1) % FRAME_SIZE;
        }
    }
}
