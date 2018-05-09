#include <stdio.h>
#include <portaudio.h>
#include <math.h>

#include "audio.h"
#include "../params.h"
#include "../utils/util.h"

PaError err;
PaStream *stream;

/* This routine will be called by the PortAudio engine when audio is needed.
It may called at interrupt level on some machines so don't do anything
that could mess up the system like calling malloc() or free().
*/
static int audioCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData
) {
    audioData *data         = (audioData*)userData;
    float *out              = (float*)outputBuffer;

    (void) timeInfo;
    (void) statusFlags;
    (void) inputBuffer;
    for(unsigned long i = 0; i<framesPerBuffer;i++){
        *out++ = data->left_data[data->lastFrame][i];
        *out++ = data->right_data[data->lastFrame][i];
    }
    data->lastFrame     = (data->lastFrame + 1) % FRAME_SIZE;

    return paContinue;
}

int initAudioStream(audioData *dataPointer) {
    err =   Pa_Initialize();
    if( err != paNoError ) { handle_pa_error(err); return -1; }

    PaStreamParameters outputParameters;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */

    if( err != paNoError ) { handle_pa_error(err); return -1; }

    outputParameters.channelCount               = 2;       /* stereo output */
    outputParameters.sampleFormat               = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency           = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo  = NULL;

    err =   Pa_OpenStream(
        &stream,
        0,
        &outputParameters,
        SAMPLE_RATE,
        BUFFER_SIZE,
        paClipOff,
        audioCallback,
        dataPointer
    );

    if( err != paNoError ) { handle_pa_error(err); return -1;}

    err = Pa_StartStream( stream );
    if( err != paNoError ) { handle_pa_error(err); return -1;}

    return 1;
}

int closeAudioStream() {
    err = Pa_CloseStream( stream );
    if( err != paNoError ) { handle_pa_error(err); return -1;}

    err =   Pa_Terminate();
    if( err != paNoError ) {
        handle_pa_error(err);
        return -1;
    }

    return 1;
}
