/**
 * Desc:
 *	Class Definition for Wave object handler
 *
 * Author:
 *	Rediet Worku
 *
 * Date:
 *	4th of August, Monday.
 */
#pragma once

//========================================================================|
//			INCLUDES
//========================================================================|
#include <vector>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <string>



//========================================================================|
//			CLASS
//========================================================================|
class WaveForge
{
public:

	void Write_Wav(const char* filename);
	void Compute_Melody(const std::vector<std::string>& melody);
	void Add_Note_At(float freq, float start_time, float duration);
	void Build_Harmony();

	size_t Get_Sample_Size() const;
	int16_t Get_Sample(size_t index) const;

	void Set_Amplitude(int amplitude = 8000);
	int Get_Amplitude() const;
	void Set_Sample_Rate(int sr = 44100);
	int Get_Sample_Rate() const;
	float Get_Duration() const;

private:

	// some constants like sample rate and volume/amplitude and stuff
	int sample_rate = 44'100;
	const float DURATION = 0.4f;
	int amplitude = 8000;
	const int CHANNELS = 1;
	const int BITS_PER_SAMPLE = 16;

	std::vector<int16_t> samples;		// the audio

	// helper
	float Note_Frequency(const std::string& note);
};