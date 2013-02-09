#ifndef _pa_callback_
#define _pa_callback_

#include <sndfile.h>
#include <portaudio.h>

struct SoundData
{
  SNDFILE *sndFile;
  SF_INFO sfInfo;
  int position;
  int* buff;
  int SPEED_MULTIPLIER;
  int SPEED_DIVISOR;
};

extern void NormalSpeed(int* cursor, SoundData* data, int theRead);
extern void DoubleSpeed(int* cursor, SoundData* data, int theRead);
extern void HalfSpeed(int* cursor, SoundData* data, int theRead);

extern int Callback( 	  const void *inputBuffer, 
						            void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData );

#endif