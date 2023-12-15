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
	float frequency;
	float amplitude;
	float phase;
	float dutyCycle;  // For square waves
	int isNoise;      // Indicates if it's a noise channel
	float noiseValue; // Current value for the noise channel
} Channel;

Channel ch1;

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

	float* in = (uint8_t*)inputBuffer;
	float* out = (uint8_t*)outputBuffer;

	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		/*
		// Square wave
		float sample = (ch1.phase < ch1.dutyCycle) ? 1.0 : -1.0;
		out[i] = ch1.amplitude * sample;
		ch1.phase += ch1.frequency / SAMPLE_RATE;
		if (ch1.phase > 1.0)
		{
			ch1.phase -= 1.0;
		}*/
	}

	return 0;
}

int start_audio()
{
	srand(time(NULL));

	ch1.frequency = 440.0;
	ch1.amplitude = 0.05;
	ch1.phase = 0.0;
	ch1.dutyCycle = .50;

	PaError err = Pa_Initialize();
	if (err != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    /* Open an audio I/O stream. */
	err = Pa_OpenDefaultStream(&stream,
		0,
		1,
		paFloat32,
		SAMPLE_RATE,
		256,
		patestCallback,
		NULL);

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