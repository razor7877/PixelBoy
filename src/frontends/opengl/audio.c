#include <stdint.h>
#include <string.h>

#include "portaudio.h"

#include "frontends/opengl/audio.h"
#include "apu.h"
#include "io.h"

#define SAMPLE_RATE (44100)

typedef struct
{
	uint8_t left_phase;
	uint8_t right_phase;
}
paTestData;

PaStream* stream;

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
*/
static int patestCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	/* Cast data passed through stream to our structure. */
	paTestData* data = (paTestData*)userData;
	uint8_t* in = (uint8_t*)inputBuffer;
	uint8_t* out = (uint8_t*)outputBuffer;

	(void)inputBuffer; /* Prevent unused variable warning. */

	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		// If audio master control enabled
		if (NR52 & 0b10000000)
		{
			/*
			*out++ = data->left_phase;
			*out++ = data->right_phase;
			// Generate simple sawtooth phaser that ranges between -1.0 and 1.0.
			data->left_phase += 1;
			// When signal reaches top, drop back down
			if (data->left_phase >= 255) data->left_phase -= 255;
			// higher pitch so we can distinguish left and right
			data->right_phase += 5;
			if (data->right_phase >= 255) data->right_phase -= 255;
			*/

			if (i < (framesPerBuffer / 2))
			{
				data->left_phase = 255;
				data->right_phase = 255;
			}
			else
			{
				data->left_phase = 0;
				data->right_phase = 0;
			}
		}
		else
		{
			data->left_phase = 0;
			data->right_phase = 0;
		}

		*out++ = data->left_phase;
		*out++ = data->right_phase;
	}

	printf("Audio buffer callback\n");
	return 0;
}

int start_audio()
{
	PaError err = Pa_Initialize();
	if (err != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    static paTestData data;

    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream,
        0,          /* no input channels */
        2,          /* stereo output */
        paUInt8,  /* 32 bit floating point output */
        SAMPLE_RATE,
		64,        /* frames per buffer, i.e. the number
                           of sample frames that PortAudio will
                           request from the callback. Many apps
                           may want to use
                           paFramesPerBufferUnspecified, which
                           tells PortAudio to pick the best,
                           possibly changing, buffer size.*/
        patestCallback, /* this is your callback function */
        &data); /*This is a pointer that will be passed to
                           your callback*/
    if (err != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	// Start the audio stream
	err = Pa_StartStream(stream);
	if (err != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	printf("Started audio successfully\n");

	return 0;
}

int stop_audio()
{
	PaError err = Pa_CloseStream(stream);
	if (err != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	err = Pa_Terminate();
	if (err != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	printf("Stopped audio successfully\n");

	return 0;
}