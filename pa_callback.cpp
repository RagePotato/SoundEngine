#include "pa_callback.h"
//#include <sndfile.h>
//#include <portaudio.h>
#include <cmath>

void NormalSpeed(int* cursor, SoundData* data, int theRead){
  sf_readf_int(data->sndFile, data->buff, theRead);
    for(int i=0; i<theRead*2;i++){
      cursor[i]=data->buff[i];
    }
}

void DoubleSpeed(int* cursor, SoundData* data, int theRead){
  sf_readf_int(data->sndFile, data->buff, 2*theRead);
    for(int i=0; i<theRead;i++){
      cursor[2*i]=data->buff[i*4];
      cursor[2*i+1]=data->buff[i*4+1];
    }
}
void HalfSpeed(int* cursor, SoundData* data, int theRead){
  sf_readf_int(data->sndFile, data->buff, theRead/2);
    for(int i=0; i<theRead/2;i++){
      cursor[i*4]=(data->buff[i*2]);//divide/multiply to decrease/increase volume
      cursor[i*4 +1]=(data->buff[i*2 +1]);
      cursor[i*4 +2]=(data->buff[i*2]);
      cursor[i*4 +3]=(data->buff[i*2 +1]);
    }
}


int Callback( 	  const void *inputBuffer, 
						            void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData )
{
    (void) inputBuffer;

    SoundData* data = (SoundData*)userData; 
    int* cursor;
    int* out = (int*)outputBuffer;
    int thisSize= framesPerBuffer;
    int thisRead;

    cursor = out;
   while (thisSize>0){
    	sf_seek(data->sndFile, data->position, SEEK_SET);
      if (thisSize > (data->sfInfo.frames - data->position)*data->SPEED_DIVISOR/data->SPEED_MULTIPLIER){//is this right?
          thisRead = (data->sfInfo.frames - data->position)*data->SPEED_DIVISOR/data->SPEED_MULTIPLIER;
          data->position = 0;
      }
    	else{
    		thisRead = thisSize;
        data->position += thisRead*data->SPEED_MULTIPLIER/data->SPEED_DIVISOR;
    	}

      if((float)data->SPEED_MULTIPLIER/data->SPEED_DIVISOR <=0.5)
        HalfSpeed(cursor, data, thisRead);
      else if((float)data->SPEED_MULTIPLIER/data->SPEED_DIVISOR <=1)
        NormalSpeed(cursor, data, thisRead);
      else if((float)data->SPEED_MULTIPLIER/data->SPEED_DIVISOR <=2)
        DoubleSpeed(cursor, data, thisRead);

    	cursor += thisRead;
    	thisSize -= thisRead;
    }

  return paContinue;
}
