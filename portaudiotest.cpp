#include <portaudio.h>
#include <sndfile.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <limits>
//#include <climits>
#include <cmath>

/**
  *Todo:  FIX CORE DUMP ERROR
  *       Break up callback into non-memory-(de)allocating functions
  *       Add speed switch
  *       Add input to change switches
  *       Carry out number of remaining samples before end of buffer so I can divide speed by more than just powers of 2 without weird pauses
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
};

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
      //2X SPEED
    	/*if (thisSize > (data->sfInfo.frames - data->position)/2){
      		thisRead = (data->sfInfo.frames - data->position)/2;
      		data->position = 0;
    	}*/
      //1/2
      if (thisSize > (data->sfInfo.frames - data->position)*2){
          thisRead = (data->sfInfo.frames - data->position)*2;
          data->position = 0;
      }
    	else{
    		thisRead = thisSize;
        //2X SPEED
    		//data->position += thisRead*2;
        //1/2X SPEED
        data->position += thisRead/2;
    	}
      //DEFAULT
      //sf_readf_int(data->sndFile, cursor, thisRead);
      //2X SPEED
      /*sf_readf_int(data->sndFile, data->buff, 2*thisRead);
      for(int i=0; i<thisRead;i++){
        cursor[2*i]=data->buff[i*4];
        cursor[2*i+1]=data->buff[i*4+1];
        //cursor[2*i+2]=data->buff[i*4+2];
        //cursor[2*i+3]=data->buff[i*4+3];
        //cursor[i]=data->buff[i];
      }*/
      //1/2X SPEED
      sf_readf_int(data->sndFile, data->buff, thisRead/2);
      for(int i=0; i<thisRead/2;i++){
        cursor[i*4]=(data->buff[i*2]);//divide/multiply to decrease/increase volume
        cursor[i*4 +1]=(data->buff[i*2 +1]);
        cursor[i*4 +2]=(data->buff[i*2]);
        cursor[i*4 +3]=(data->buff[i*2 +1]);
        //cursor[2*i+2]=data->buff[i*4+2];
        //cursor[2*i+3]=data->buff[i*4+3];
        //cursor[i]=data->buff[i];
      }
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

int main(int argc, char* argv[]){

	char* filename= (char*)argfunc(argc,argv);                    if(filename==NULL) return -1;

	SoundData *data = (SoundData *)malloc(sizeof(SoundData));
  data->buff = (int *)malloc(sizeof(int)*64*2);//frames*speedup*channels
	PaStream *stream;
	PaError err;
	PaStreamParameters outputParameters;

	data->position = 0;
  data->sfInfo.format = 0;

  try{
    data->sndFile = sf_open(filename, SFM_READ, &data->sfInfo); if (!data->sndFile) throw data;

  	try{
  		err = Pa_Initialize(); 			                              if (err != paNoError) throw err;
  		outputParameters.device = Pa_GetDefaultOutputDevice();
  		outputParameters.channelCount = data->sfInfo.channels;
  		outputParameters.sampleFormat = paInt32;
  		outputParameters.suggestedLatency = 1.6;
  		outputParameters.hostApiSpecificStreamInfo = 0;
  		err = Pa_OpenStream(&stream,
  		                        0,
  		                        &outputParameters,
  		                        data->sfInfo.samplerate,
  		                        32,
  		                        paNoFlag,
  		                        Callback,
  		                        data );                           if (err != paNoError) throw err;
      err = Pa_StartStream( stream );                           if (err != paNoError) throw err;
      std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
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








