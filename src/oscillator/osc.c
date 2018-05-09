#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "osc.h"
#include "../params.h"
#include "../params.h"

#define sawCount    6
#define sqrCount    6
#define triCount    6

float wavetable[WAVE_COUNT][TABLE_SIZE];

float oscillate_sin(float time, float freq, float phase) {
    return sin ( freq * 2 * M_PI * time + phase * M_PI);
}

float oscillate_saw(float time, float freq, float phase) {
    float output    = 1/2;

    for (unsigned int i = 1; i <= sawCount; i++) {
        output      -= powf(-1.0, (float) i) * sin ( i * freq * 2 * M_PI * time + phase * M_PI) / i;
    }
    return output;
}

float oscillate_sqr(float time, float freq, float phase) {
    float output    = 0;

    for (unsigned int i = 1; i <= sawCount; i++) {
        output      += sin((2 * M_PI * (2 * i - 1) * freq * time) / (2 * i - 1) + phase * M_PI);
    }
    return output;
}

float oscillate_tri(float time, float freq, float phase) {
    float output    = 0;

    for (unsigned int i = 1; i <= sawCount; i++) {
        output      += cos(((2 * M_PI * (2 * i - 1) * freq * time) / pow(2 * i - 1, 2)) + phase * M_PI);
    }
    return output;
}

int readToWavetable(int waveIndex, char filePath[]) {
    char *record,*line;
    FILE *fstream   = fopen(filePath, "r");
    unsigned int j  = 0;
    char buffer[10];

    if(fstream == NULL) {
        printf("\n----file opening failed----\n");
        return -1;
    }

    line    = fgets(buffer,sizeof(buffer),fstream);
    record  = strtok(line,",");

    while(record != NULL) {
        wavetable[waveIndex][j++]   = atof(record) ;
        record                      = strtok(NULL,",");
        printf("%f|", wavetable[waveIndex][j++]);
    }
    return 0 ;
}
