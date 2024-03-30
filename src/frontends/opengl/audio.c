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

#define SAMPLE_RATE (44100)

typedef struct
{
	float frequency;
	float amplitude;
	float phase;
	float dutyCycle;  // For square waves
	int isNoise;      // Indicates if it's a noise channel
	float noiseValue; // Current value for the noise channel
} PAChannel;

// Square channel 1
PAChannel PA_NR1 = {
	440.0,
	0.05,
	0.0,
	0.50
};

// Square channel 2
PAChannel PA_NR2 = {
	440.0,
	0.05,
	0.0,
	0.50
};

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

	float* in = (uint8_t*)inputBuffer;
	float* out = (uint8_t*)outputBuffer;

	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		out[i] = 0;

		if (NR52 & AUDIO_ON)
		{
			// Square channel 1
			if (NR52 & CH1_ON)
			{
				if (NR1.r2 & 0xF8 == 0)
					log_debug("NR12 DAC off\n");
				//printf("NR12 volume: %d - Curr volume: %d\n", (NR1.r2 & 0b11110000) >> 4, NR1.volume);
				uint16_t period_value = NR1.r3 | ((NR1.r4 & 0b111) << 8);
				float gb_frequency = 131072.0 / (2048 - period_value);
				float gb_sample_rate = 1048576.0 / (2048 - period_value);

				uint8_t duty_cycle = (NR1.r1 & WAVE_DUTY) >> 6;
				PA_NR1.dutyCycle = duty_cycle == 0 ? 0.125 : (duty_cycle * 0.25);

				PA_NR1.amplitude = NR1.volume / 15.0;

				//printf("Frequency, %f -- Sample rate, %f\n", frequency, gb_sample_rate);
				PA_NR1.frequency = gb_frequency;
				// Square wave
				float sample = (PA_NR1.phase < PA_NR1.dutyCycle) ? 1.0 : -1.0;
				out[i] += PA_NR1.amplitude * sample;
				PA_NR1.phase += PA_NR1.frequency / SAMPLE_RATE;
				if (PA_NR1.phase > 1.0)
				{
					PA_NR1.phase -= 1.0;
				}
			}
			// Square channel 2
			if (NR52 & CH2_ON)
			{
				if (NR2.r2 & 0xF8 == 0)
					log_debug("NR22 DAC off\n");
				//printf("NR22 volume: %d - Curr volume: %d\n", (NR2.r2 & 0b11110000) >> 4, NR2.volume);
				uint16_t period_value = NR2.r3 | ((NR2.r4 & 0b111) << 8);
				float gb_frequency = 131072.0 / (2048 - period_value);
				float gb_sample_rate = 1048576.0 / (2048 - period_value);

				uint8_t duty_cycle = (NR2.r1 & WAVE_DUTY) >> 6;
				PA_NR2.dutyCycle = duty_cycle == 0 ? 0.125 : (duty_cycle * 0.25);

				PA_NR2.amplitude = NR2.volume / 15.0;

				//printf("Frequency, %f -- Sample rate, %f\n", frequency, gb_sample_rate);
				PA_NR2.frequency = gb_frequency;
				// Square wave
				float sample = (PA_NR2.phase < PA_NR2.dutyCycle) ? 1.0 : -1.0;
				out[i] += PA_NR2.amplitude * sample;
				PA_NR2.phase += PA_NR2.frequency / SAMPLE_RATE;
				if (PA_NR2.phase > 1.0)
				{
					PA_NR2.phase -= 1.0;
				}
			}
			// Wave channel
			if (NR52 & CH3_ON)
			{

			}
			// Noise channel
			if (NR52 & CH4_ON)
			{

			}
		}
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
		SAMPLE_RATE,
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