
typedef struct {
    float left_data;
    float right_data;
} Sample;

typedef struct {
    //--- 1 - The audio engine is running | 0 - it's not
    int engineRunning;
    //--- The frequency for the currently played midi note
    float frequency;
    //--- Phase values for each waveform
    float phase[WAVE_COUNT];
    //--- Amplitude values for each waveform
    float ampWave[WAVE_COUNT];
    //--- Current part of a second => current
    float time;
} ControlStatus;

Sample  calculateSample(float);
void    initEngine(ControlStatus *);
