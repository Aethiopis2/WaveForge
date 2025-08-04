# WaveForge

WaveForge is a simple Windows application for generating, visualizing, and playing musical waveforms. It allows users to create melodies and chords, visualize the resulting waveform, and save the output as a standard WAV audio file.

## Features

- **Generate Chords:** Quickly add C Major, G Major, or D Minor chords.
- **Play Melody:** Listen to the generated audio directly in the app.
- **Waveform Visualization:** See a real-time plot of the audio waveform.
- **WAV Export:** Save your creation as a `.wav` file for use in other applications.

---

## Getting Started

### Prerequisites

- Windows OS
- Visual Studio 2022 (or compatible)
- C++14 compatible compiler

### Building

1. Open `WaveForge.sln` in Visual Studio.
2. Build the solution (`Ctrl+Shift+B`).
3. Run the application (`F5`).

---

## Usage

- Click **Play Melody** to play the current melody.
- Click **C Major**, **G Major**, or **D Minor** to add those chords to your composition and play them.
- The waveform display updates in real time as you play or add notes.

---

## The WAV File Format

WaveForge saves audio in the standard [WAV](https://en.wikipedia.org/wiki/WAV) format. WAV is a subset of the RIFF file format and is widely supported for uncompressed audio.

### WAV File Structure

Below is a visual breakdown of a typical WAV file's structure:


#### 1. RIFF Header

| Offset | Size | Description         |
|--------|------|---------------------|
| 0      | 4    | "RIFF"              |
| 4      | 4    | File size - 8 bytes |
| 8      | 4    | "WAVE"              |

#### 2. "fmt " Subchunk

| Offset | Size | Description         |
|--------|------|---------------------|
| 12     | 4    | "fmt "              |
| 16     | 4    | Subchunk size (16)  |
| 20     | 2    | Audio format (1=PCM)|
| 22     | 2    | Num channels        |
| 24     | 4    | Sample rate         |
| 28     | 4    | Byte rate           |
| 32     | 2    | Block align         |
| 34     | 2    | Bits per sample     |

#### 3. "data" Subchunk

| Offset | Size | Description         |
|--------|------|---------------------|
| 36     | 4    | "data"              |
| 40     | 4    | Data size           |
| 44     | ...  | Audio data          |

---

### Visual Diagram

Below is a diagram illustrating the WAV file structure:

![WAV File Format Diagram](https://upload.wikimedia.org/wikipedia/commons/4/4b/Wav_file_structure.png)

*Image credit: [Wikipedia - WAV](https://en.wikipedia.org/wiki/WAV)*

---

## License

This project is provided for educational purposes.

---

## Author

Rediet Worku
