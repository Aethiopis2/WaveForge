/**
 * Desc: 
 *	program entry point.
 *
 * Author: 
 *	Rediet Worku
 *
 * Date: 
 *	4th of August, Monday.
 */
#pragma comment(lib, "winmm.lib")
#define WIN32_LEAN_AND_MEAN


//========================================================================|
//			INCLUDES
//========================================================================|
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#include "WaveForge.hpp"



//========================================================================|
//			DEFINES
//========================================================================|
// Window props
#define WINDOW_TITLE "WaveForge v1.0.0"
#define CLASS_NAME   "WaveForgeWindowClass"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 400

// start at the center of screen
#define WINDOW_X ((GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2)
#define WINDOW_Y ((GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2)


// button props
#define BUTTON_CAPTION "Play Melody"
#define BUTTON_X 20
#define BUTTON_Y WINDOW_HEIGHT - 70
#define BUTTON_STYLE WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON
#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 30


//========================================================================|
//			GLOBALS
//========================================================================|
char err_text[255];		// generalized error buffer
int btn_id = 1;
char file_name[256] = "melody.wav";		// a default file name

WaveForge waveforge;	// WaveForge object to handle audio
RECT g_waveRect = { 20, 20, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 80 };

UINT_PTR timer_id = 1;
size_t play_head = 0; // current sample index
const int TIMER_INTERVAL_MS = 20; // ~50fps




//========================================================================|
//			FUNCTIONS
//========================================================================|
/**
 * Desc:
 *	Draw the waveform of the audio samples in the specified rectangle
 *
 * Params:
 *  hdc: the device context to draw on
 *  rect: the rectangle where the waveform will be drawn
 */
void Draw_Waveform(HDC hdc, const RECT& rect, size_t offset, size_t visible_len)
{
	size_t sample_size = waveforge.Get_Sample_Size();
	if (sample_size == 0 || offset >= sample_size) return;

	int mid_y = rect.top + (rect.bottom - rect.top) / 2;
	int height = rect.bottom - rect.top;
	int width = rect.right - rect.left;

	// Step 1: Find peak amplitude in visible range
	int16_t max_sample = 1; // Avoid divide by zero
	size_t end = min(offset + visible_len, sample_size);
	for (size_t i = offset; i < end; ++i)
	{
		int16_t sample = waveforge.Get_Sample(i);
		max_sample = std::max<int16_t>(max_sample, std::abs(sample));
	}

	// Step 2: Draw waveform
	MoveToEx(hdc, rect.left, mid_y, nullptr);
	for (size_t x = 0; x < width && offset + x < end; ++x)
	{
		int16_t sample = waveforge.Get_Sample(offset + x);
		int y = mid_y - (sample * (height / 2)) / max_sample;
		LineTo(hdc, rect.left + x, y);
	}
}


//========================================================================|
/**
 * Desc: 
 *	Main window proc
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	static HDC hdc;
	PAINTSTRUCT ps;

	switch (msg) 
	{
	case WM_CREATE:
	{
		HWND hButton = CreateWindow("BUTTON", BUTTON_CAPTION, BUTTON_STYLE,
			BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)btn_id, NULL, NULL);
		
		HWND hChordC = CreateWindow("BUTTON", "C Major", BUTTON_STYLE,
			160, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)2, NULL, NULL);

		HWND hChordG = CreateWindow("BUTTON", "G Major", BUTTON_STYLE,
			300, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)3, NULL, NULL);

		HWND hChordDmin = CreateWindow("BUTTON", "D Minor", BUTTON_STYLE,
			440, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)4, NULL, NULL);

		if (!hChordC || !hChordG || !hChordDmin || !hButton)
		{
			snprintf(err_text, 255, "Unable to create chord buttons, error code: %d",
				GetLastError());
			MessageBox(hwnd, err_text, "Error Box", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		} // end if
	} return 0;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 1: // "Play Melody"
			if (!PlaySound(file_name, NULL, SND_FILENAME | SND_ASYNC))
			{
				snprintf(err_text, 255, "Unable to play file: %s", file_name);
				MessageBox(hwnd, err_text, "Error Box", MB_OK | MB_ICONERROR);
			}
			break;

		case 2: // C Major
			waveforge.Add_Note_At(261.63f, waveforge.Get_Duration(), 0.4f); // C4
			waveforge.Add_Note_At(329.63f, waveforge.Get_Duration(), 0.4f); // E4
			waveforge.Add_Note_At(392.00f, waveforge.Get_Duration(), 0.4f); // G4

			PlaySound(NULL, NULL, SND_PURGE);           // Stop current playback
			waveforge.Write_Wav(file_name);             // Save new wave
			PlaySound(file_name, NULL, SND_FILENAME | SND_ASYNC); // Replay new audio
			break;

		case 3: // G Major
			waveforge.Add_Note_At(392.00f, waveforge.Get_Duration(), 0.4f); // G4
			waveforge.Add_Note_At(493.88f, waveforge.Get_Duration(), 0.4f); // B4
			waveforge.Add_Note_At(587.33f, waveforge.Get_Duration(), 0.4f); // D5

			PlaySound(NULL, NULL, SND_PURGE);           // Stop current playback
			waveforge.Write_Wav(file_name);             // Save new wave
			PlaySound(file_name, NULL, SND_FILENAME | SND_ASYNC); // Replay new audio
			break;

		case 4: // D Minor
			waveforge.Add_Note_At(293.66f, waveforge.Get_Duration(), 0.4f); // D4
			waveforge.Add_Note_At(349.23f, waveforge.Get_Duration(), 0.4f); // F4
			waveforge.Add_Note_At(440.00f, waveforge.Get_Duration(), 0.4f); // A4

			PlaySound(NULL, NULL, SND_PURGE);           // Stop current playback
			waveforge.Write_Wav(file_name);             // Save new wave
			PlaySound(file_name, NULL, SND_FILENAME | SND_ASYNC); // Replay new audio
			break;
		}

		SetTimer(hwnd, timer_id, TIMER_INTERVAL_MS, NULL); // start the timer for playback
		InvalidateRect(hwnd, &g_waveRect, TRUE);    // Redraw waveform
		UpdateWindow(hwnd);
	} return 0;

	case WM_TIMER:
	{
		size_t sample_len = waveforge.Get_Sample_Size();
		if (sample_len == 0)
			break;

		play_head += waveforge.Get_Sample_Rate() / 50; // advance ~1/50th sec per tick

		if (play_head >= sample_len)
		{
			KillTimer(hwnd, timer_id);
			play_head = 0;
		}
		InvalidateRect(hwnd, &g_waveRect, TRUE); // trigger repaint
	} return 0;


	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		Rectangle(hdc, g_waveRect.left - 1, g_waveRect.top - 1, g_waveRect.right + 1, g_waveRect.bottom + 1);
		Draw_Waveform(hdc, g_waveRect, play_head, 800); // show 400 samples
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PlaySound(NULL, NULL, SND_PURGE);
		PostQuitMessage(0);
		return 0;


	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	} // end switch
} // end WndProc


//========================================================================|
/**
 * Desc:
 *	Program entry point
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// write the melody file
	/*std::vector<std::string> melody = { "C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5" };
	waveforge.Compute_Melody(melody);*/
	waveforge.Build_Harmony();
	waveforge.Write_Wav(file_name);


	// Register the window class
	WNDCLASSEX wc = {};
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // Use default window background color
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Use default arrow cursor
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = CLASS_NAME;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;

	if (!RegisterClassEx(&wc))
		return -1; // Registration failed

	// Create the window
	HWND hwnd;
	if (!(hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_CAPTION | WS_SYSMENU,
		WINDOW_X, WINDOW_Y,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL)))
		return -1;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end while

	return (int)msg.wParam;
} // end WinMain