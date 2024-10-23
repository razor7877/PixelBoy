#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "portaudio.h"

#include "frontends/opengl/audio.h"
#include "apu.h"
#include "io.h"
#include "cpu.h"
#include "frontends/opengl/frontend.h"
#include "logging.h"

PaStream* stream;

double current = 0;
double delta = 0.0f;
double last = 0.0f;

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
*/
static int paCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	current = glfwGetTime();
	delta = current - last;
	last = current;

	float* in = inputBuffer;
	float* out = outputBuffer;

	int sample_index = apu_state.sample_index;

	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		out[i] = apu_state.sample_buffer[sample_index];
		sample_index = (sample_index + 1) % APU_SAMPLE_BUFFER_SIZE;
	}

	return 0;
}

int start_audio()
{
	srand(time(NULL));

	PaError err = Pa_Initialize();
	if (err != paNoError)
		log_error("PortAudio error: %s\n", Pa_GetErrorText(err));

    /* Open an audio I/O stream. */
	err = Pa_OpenDefaultStream(&stream,
		0,
		1,
		paFloat32,
		TARGET_SAMPLE_RATE,
		256,
		paCallback,
		NULL);

    if (err != paNoError)
		log_error("PortAudio error: %s\n", Pa_GetErrorText(err));

	// Start the audio stream
	err = Pa_StartStream(stream);
	if (err != paNoError)
		log_error("PortAudio error: %s\n", Pa_GetErrorText(err));

	log_info("Started audio successfully\n");

	return 0;
}

int stop_audio()
{
	PaError err = Pa_CloseStream(stream);
	if (err != paNoError)
		log_error("PortAudio error: %s\n", Pa_GetErrorText(err));

	err = Pa_Terminate();
	if (err != paNoError)
		log_error("PortAudio error: %s\n", Pa_GetErrorText(err));

	log_info("Stopped audio successfully\n");

	return 0;
}
