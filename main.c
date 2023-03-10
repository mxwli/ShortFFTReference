#include "raylib.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "complex.h"
#define dc double complex
#define MAXL 12
#define MAXN (1<<MAXL)
#define sW 1000
#define sH 500

dc fft[2][MAXN];
void transform() {
	dc root = cexp(2*I*3.14159265358979/MAXN);
	for(int i = MAXL-1; i >= 0; i--) {
		dc it = cpow(root, (dc)(1<<i)), accum = 1/it;
		for(int i2 = 0; i2 < MAXN/2; i2++) {
			if(i2%(1<<i) == 0) accum *= it;
			fft[i&1][i2] = 		fft[i+1&1][i2/(1<<i)*(2<<i)+i2%(1<<i)] + accum * fft[i+1&1][i2/(1<<i)*(2<<i)+i2%(1<<i)+(1<<i)];
			fft[i&1][i2+MAXN/2] = 	fft[i+1&1][i2/(1<<i)*(2<<i)+i2%(1<<i)] - accum * fft[i+1&1][i2/(1<<i)*(2<<i)+i2%(1<<i)+(1<<i)];
		}
	}
}

int main() {
	InitWindow(sW, sH, "fft");
	InitAudioDevice();
	char str[100];
	scanf("%s", str);
	Wave wave = LoadWave(str);	
	Sound sound = LoadSound(str);
	
	float* data = LoadWaveSamples(wave);
	int rate = wave.sampleRate, frames = wave.frameCount, depth = wave.sampleSize/8, channels = wave.channels;

	PlaySound(sound);
	float start = GetTime();

	SetTargetFPS(30);
	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(10, 10);

		float time = GetTime()-start;
		int pos = (int)((time-MAXN/rate)*rate)*channels;
		if(pos<0) pos = 0;
		for(int i = pos, j = 0; i < channels*frames && i < pos + MAXN*channels; i += channels, j++) {
			fft[MAXL&1][j] = (dc)data[i];
		}

		transform();
		
		for(int i = 1; i < MAXN; i++) {
			float xpos = log(i)/log(MAXN)*sW, pxpos = log(i>0?i-1:i)/log(MAXN)*sW;
			DrawLine((int)xpos, sH-10-cabs(fft[0][i]), (int)pxpos, sH-10-cabs(fft[0][i-1]), DARKGRAY);
		}
		
		EndDrawing();
	}

	StopSound(sound);
	UnloadWaveSamples(data);
	UnloadWave(wave);
	UnloadSound(sound);

	CloseAudioDevice();
	CloseWindow();
	return 0;
}

