#include <portaudio.h>
#include <sndfile.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <limits>
//#include <climits>
#include <cmath>
#include <cstring>

/**
  *Todo:  FIX CORE DUMP ERROR ? does this exist anymore?
  *       Use regex for input and output
  *       Carry out number of remaining samples before end of buffer so I can divide speed by more than just powers of 2 without weird pauses
  *       Add negative speeds
  *       Add rational speed switching insted of just dividing/multiplying by integers
  *       Change speed switch to speed slide
  *       Break up into multiple files
  *       Add 3d sound
  *       Add reverberation
  *       Add sound reflection
  *       Add damping
  */

struct SoundData
{
  SNDFILE *sndFile;
  SF_INFO sfInfo;
  int position;
  int* buff;
  int SPEED_MULTIPLIER;
  int SPEED_DIVISOR;
};

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


static int Callback( 	  const void *inputBuffer, 
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

void* argfunc(int argc, char* argv[]){
  if(argc!=2){
    printf("usage: audiotest [audio file]\n");
    return NULL;
  }
  char* filename=argv[1];
  return filename;
}

int commandfunc(char input[], SoundData* data){
  if(strcmp(input,"quit")==0)
    return 1;
  else if(strcmp(input,"help")==0){
    printf("\tquit:\tquit engine\n");
    printf("\thelp:\tdisplat this menu\n");
    printf("\t2:\trun at double speed\n");
    printf("\t1/2:\trun at half speed\n");
    printf("\t1:\trun at normal speed\n");
  }
  else if(strcmp(input,"1")==0){
    data->SPEED_MULTIPLIER=1;
    data->SPEED_DIVISOR=1;
  }
  else if(strcmp(input,"2")==0){
    data->SPEED_MULTIPLIER=2;
    data->SPEED_DIVISOR=1;
  }
  else if(strcmp(input,"1/2")==0){
    data->SPEED_MULTIPLIER=1;
    data->SPEED_DIVISOR=2;
  }
  return 0;
}

int main(int argc, char* argv[]){

	char* filename= (char*)argfunc(argc,argv);                    if(filename==NULL) return -1;

	SoundData *data = (SoundData *)malloc(sizeof(SoundData));
  data->buff = (int *)malloc(sizeof(int)*64*2);//frames*speedup*channels
	data->SPEED_MULTIPLIER=1;
  data->SPEED_DIVISOR=1;
  PaStream *stream;
	PaError err;
	PaStreamParameters outputParameters;

  char inputBuffer[256];

	data->position = 0;
  data->sfInfo.format = 0;

  int end=0;

  try{
    data->sndFile = sf_open(filename, SFM_READ, &data->sfInfo); if (!data->sndFile) throw data;

  	try{
      
  		err = Pa_Initialize(); 			                              if (err != paNoError) throw err;
  		outputParameters.device = Pa_GetDefaultOutputDevice();
  		outputParameters.channelCount = data->sfInfo.channels;
  		outputParameters.sampleFormat = paInt32;
  		outputParameters.suggestedLatency = 3.2;
  		outputParameters.hostApiSpecificStreamInfo = 0;
  		err = Pa_OpenStream(&stream,
  		                        0,
  		                        &outputParameters,
  		                        data->sfInfo.samplerate,
  		                        4,
  		                        paNoFlag,
  		                        Callback,
  		                        data );                           if (err != paNoError) throw err;
      err = Pa_StartStream( stream );                           if (err != paNoError) throw err;
      while(!end){
        std::cin.getline (inputBuffer,256);
        end = commandfunc(inputBuffer, data);
      }
      err = Pa_StopStream( stream ); 	                          if (err != paNoError) throw err;
      err = Pa_CloseStream( stream );                           if (err != paNoError) throw err;//Pa_AbortStream() is faster
      err = Pa_Terminate(); 			                              if (err != paNoError) throw err;
  	}
    catch(PaError e){
  		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
  		return 1;
  	}

    int sferr= sf_close(data->sndFile);                         if (sferr!=0) throw sferr;
	
  }
  catch(SoundData *e){
    printf("SounfFile error: %s\n", sf_strerror(e->sndFile));
    free(data->buff);
    free (data);
    return 2;
  }
  catch(int e){
    printf("SounfFile error: %s\n", sf_error_number(e));
    free(data->buff);
    free (data);
    return 2;
  }

  free(data->buff);
  free (data);
  return 0;
}








