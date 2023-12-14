#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "portaudio.h"

#include "frontends/opengl/audio.h"
#include "apu.h"
#include "io.h"
#include <corecrt_math_defines.h>
#include "cpu.h"
#include "frontends/opengl/frontend.h"

#define SAMPLE_RATE (44100)

typedef struct
{
	float phase;
	float increment;
} paData;

static paData audio_data;
PaStream* stream;

double current = 0;
double delta = 0.0f;
double last = 0.0f;

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
	current = glfwGetTime();
	delta = current - last;
	last = current;

	/* Cast data passed through stream to our structure. */
	paData* data = (paData*)userData;
	float* in = (uint8_t*)inputBuffer;
	float* out = (uint8_t*)outputBuffer;

	(void)inputBuffer; /* Prevent unused variable warning. */

#define COUNT 16
	uint8_t count = COUNT;
	bool isHigh = false;

	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		// If audio master control enabled
		if (NR52 & 0b10000000)
		{
			//update_audio();

			//printf("%d\n", *out);
			if (count-- == 0)
			{
				isHigh = !isHigh;
				count = COUNT;
			}
			if (isHigh)
				*out++ = (float)rand() / RAND_MAX;
			else
				*out++ = (float)rand() / RAND_MAX;
		}
		else
		{
			data->phase = 0;
			*out++ = data->phase;
		}
	}

	//printf("\n\n\n\n");

	//printf("Audio buffer callback: %f\n", delta);
	return 0;
}

int start_audio()
{
	srand(time(NULL));

	PaError err = Pa_Initialize();
	if (err != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream,
        0,
        1,
        paFloat32,
        SAMPLE_RATE,
		2048,
        patestCallback, /* this is your callback function */
        &audio_data); /*This is a pointer that will be passed to
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

int update_audio()
{
	paData* data = &audio_data;

	// Square wave
	float sample = (data->phase < M_PI) ? 127 : -127;

	// Increment phase and handle wrap-around
	data->phase += 0.05;
	if (data->phase > 2.0 * M_PI)
	{
		data->phase -= 2.0 * M_PI;
	}
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