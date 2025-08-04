/**
 * Desc:
 *	Implementaion details for class WaveForge
 *
 * Author:
 *	Rediet Worku
 *
 * Date:
 *	4th of August, Monday.
 */

//========================================================================|
//			INCLUDES
//========================================================================|
#include "WaveForge.hpp"


//========================================================================|
//			DEFINES
//========================================================================|
#define PI 3.1415926f


//========================================================================|
//			METHODS
//========================================================================|
/**
 * Desc:
 *  Write the audio samples to a WAV file
 *
 * Params:
 *  filename: the name of the file to write to
 */
void WaveForge::Write_Wav(const char* filename)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
		return;

	int subchunk2_size = samples.size() * sizeof(int16_t);
	int chunk_size = 36 + subchunk2_size; // 36 + Subchunk2Size

	// Write the WAV header
	file.write("RIFF", 4);										// Chunk ID
	file.write(reinterpret_cast<char*>(&chunk_size), 4);		// Chunk Size (to be filled later)
	file.write("WAVE", 4);										// Format
	file.write("fmt ", 4);										// Subchunk1 ID (note the space after fmt!)
	
	int subchunk1_size = 16; // Subchunk1 Size for PCM
	short audio_format = 1;

	file.write(reinterpret_cast<char*>(&subchunk1_size), 4);	// Subchunk1 Size (16 for PCM)
	file.write(reinterpret_cast<char*>(&audio_format), 2);		// Audio Format (1 for PCM)

	file.write(reinterpret_cast<const char*>(&CHANNELS), 2);	// Number of Channels
	file.write(reinterpret_cast<const char*>(&sample_rate), 4); // Sample Rate

	int byte_rate = sample_rate * CHANNELS * (BITS_PER_SAMPLE / 8);
	int block_align = CHANNELS * (BITS_PER_SAMPLE / 8);
	file.write(reinterpret_cast<char*>(&byte_rate), 4);			// Byte Rate (to be filled later)
	file.write(reinterpret_cast<char*>(&block_align), 2);		// Block Align
	file.write(reinterpret_cast<const char*>(&BITS_PER_SAMPLE), 2); // Bits per sample

	file.write("data", 4);										// audio data section
	file.write(reinterpret_cast<char*>(&subchunk2_size), 4);	// Subchunk2 Size (number of samples * bytes per sample)
	file.write(reinterpret_cast<const char*>(samples.data()), subchunk2_size);	// the actual audio

	file.close();
} // end Write_Wav


//========================================================================|
/**
 * Desc:
 *  Compute the cosines of musical notes to generate sounds
 *
 * Params:
 *  melody: a vector of musical notes in text form, e.g., {"C4", "D4", "E4", ...}
 */
void WaveForge::Compute_Melody(const std::vector<std::string>& melody)
{
	samples.clear();
	for (const auto& note : melody)
	{
		float freq = Note_Frequency(note);
		int num_samples = static_cast<int>(sample_rate * DURATION);

		for (int i = 0; i < num_samples; ++i)
		{
			float t = static_cast<float>(i) / sample_rate;
			float sample = (freq == 0.0f) ? 0.0f : amplitude * std::cos(2 * 3.14159f * freq * t);
			samples.push_back(static_cast<int16_t>(sample));
		} // end for nested
	} // end outer for
} // end Compute_Melody


//========================================================================|
/**
 * Desc:
 *  Add a note at a specific frequency, start time, and duration
 *
 * Params:
 *  freq: the frequency of the note to add
 *  start_time: the time in seconds when the note starts
 *  duration: the duration of the note in seconds
 */
void WaveForge::Add_Note_At(float freq, float start_time, float duration)
{
	int start_sample = static_cast<int>(start_time * sample_rate);
	int num_samples = static_cast<int>(duration * sample_rate);

	if (samples.size() < start_sample + num_samples)
		samples.resize(start_sample + num_samples, 0);

	for (int i = 0; i < num_samples; ++i)
	{
		float t = static_cast<float>(i) / sample_rate;
		float sample = amplitude * std::cos(2 * PI * freq * t);
		
		int idx = start_sample + i;
		samples[idx] += static_cast<int16_t>(sample);
		
		// cap extremes
		if (samples[idx] > 32767) samples[idx] = 32767;
		if (samples[idx] < -32768) samples[idx] = -32768;
	} // end for
} // end Add_Note_At


//========================================================================|
/**
 * Desc:
 *  Build a harmony by adding multiple notes at specific intervals
 */
void WaveForge::Build_Harmony()
{
	samples.clear();

	float duration = 0.4f;

	Add_Note_At(261.63f, 0.0f, duration); // C4
	Add_Note_At(329.63f, 0.0f, duration); // E4
	Add_Note_At(392.00f, 0.0f, duration); // G4

	Add_Note_At(329.63f, 0.4f, duration); // E4

	Add_Note_At(392.00f, 0.8f, duration); // G4
	Add_Note_At(493.88f, 0.8f, duration); // B4
	Add_Note_At(587.33f, 0.8f, duration); // D5

	Add_Note_At(349.23f, 1.2f, duration); // F4

	Add_Note_At(293.66f, 1.6f, duration); // D4
	Add_Note_At(349.23f, 1.6f, duration); // F4
	Add_Note_At(440.00f, 1.6f, duration); // A4

	Add_Note_At(329.63f, 2.0f, duration); // E4

	Add_Note_At(392.00f, 2.4f, duration); // G4
	Add_Note_At(523.25f, 2.4f, duration); // C5
	Add_Note_At(659.25f, 2.4f, duration); // E5

	Add_Note_At(261.63f, 2.8f, duration); // C4
} // end Build_Harmony


//========================================================================|
/**
 * Desc:
 *  Get the size of the audio samples in bytes
 *
 * Returns:
 *  the size of the samples vector in bytes
 */
size_t WaveForge::Get_Sample_Size() const
{
	return samples.size() * sizeof(int16_t);
} // end Get_Sample_Size


//========================================================================|
/**
 * Desc:
 *  Get a specific audio sample by index
 *
 * Params:
 *  index: the index of the sample to retrieve
 *
 * Returns:
 *  the audio sample at the specified index, or 0 if out of bounds
 */
int16_t WaveForge::Get_Sample(size_t index) const
{
	if (index < samples.size())
		return samples[index];

	return 0; // return 0 if index is out of bounds
} // end Get_Sample


//========================================================================|
/**
 * Desc:
 *  Set the amplitude for the audio samples
 *
 * Params:
 *  amplitude: the amplitude value to set, default is 8000
 */
void WaveForge::Set_Amplitude(int amplitude)
{
	this->amplitude = amplitude;
} // end Set_Amplitude


//========================================================================|
/**
 * Desc:
 *  Get the current amplitude value
 *
 * Returns:
 *  the current amplitude value
 */
int WaveForge::Get_Amplitude() const
{
	return amplitude;
} // end Get_Amplitude


//========================================================================|
/**
 * Desc:
 *  Get the total duration of the audio samples in seconds
 *
 * Returns:
 *  the duration of the audio in seconds
 */
float WaveForge::Get_Duration() const
{
	return static_cast<float>(samples.size()) / sample_rate;
} // end Get_Duration


//========================================================================|
/**
 * Desc:
 *  Set the sample rate for the audio samples
 *
 * Params:
 *  sr: the sample rate in Hz, must be greater than 0
 */
void WaveForge::Set_Sample_Rate(int sr)
{
	if (sr > 0)
		sample_rate = sr;
} // end Set_Sample_Rate


//========================================================================|
/**
 * Desc:
 *  Get the current sample rate
 *
 * Returns:
 *  the sample rate in Hz
 */
int WaveForge::Get_Sample_Rate() const
{
	return sample_rate;
} // end Get_Sample_Rate


//========================================================================|
/**
 * Desc:
 *  Convert's the text musical notes to frequency range
 *
 * Params:
 *  note: the musical note, C4 - C5
 *
 * Returns:
 *  the frequency of the note if found alas 0
 */
float WaveForge::Note_Frequency(const std::string& note)
{
    if (note == "C4") return 261.63f;
    if (note == "D4") return 293.66f;
    if (note == "E4") return 329.63f;
    if (note == "F4") return 349.23f;
    if (note == "G4") return 392.00f;
    if (note == "A4") return 440.00f;
    if (note == "B4") return 493.88f;
    if (note == "C5") return 523.25f;
    return 0.0f;
} // end Note_Frequency