#include "../params.h"

typedef struct {
    float freq;
    float left_data[FRAME_SIZE][BUFFER_SIZE];
    float right_data[FRAME_SIZE][BUFFER_SIZE];
    unsigned int lastFrame;
} audioData;

int initAudioStream();
int closeAudioStream();
