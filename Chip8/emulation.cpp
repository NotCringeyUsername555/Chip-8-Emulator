#include "SDL.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <bitset>
#include <iostream>
#include <time.h>
#include <string>
#include <math.h>
unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16];
unsigned short I;
unsigned int pc = 0x200;
bool gfx[64][32];
unsigned short delay_timer;
unsigned short sound_timer;
unsigned short sp;
SDL_Renderer * render;
SDL_Event eventS;
int key[16];
std::ifstream fs;
SDL_Window * window;
unsigned short chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
void drawGraphics() {
	SDL_SetRenderDrawColor(render, 255, 255, 255, 0);
	SDL_RenderClear(render);
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	for (int i = 0; i < 64; i++) {
		for (int i2 = 0; i2 < 32; i2++) {
			if (gfx[i][i2]) {
				SDL_Rect r;
				r.x = i * 10;
				r.y = i2 * 10;
				r.w = 10;
				r.h = 10;
				SDL_RenderFillRect(render, &r);
			}
		}
	}
	SDL_RenderPresent(render);

}
void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Chip8", 100, 100, 640, 320, 0);
	srand(time(NULL));
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}
	for (int i = 0; i < 80; i++) {
		memory[i] = chip8_fontset[i];
	}
	for (int i = 0; i < 64; i++) {
		for (int i2 = 0; i2 < 32; i2++) {
			gfx[i][i2] = false;
		}
	}
	for (int i = 0x200; i < 4096; i++) {
		std::cout << memory[i] << " ";
	}
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

std::streampos fileSize(const char* filePath) {

	std::streampos fsize = 0;
	std::ifstream file(filePath, std::ios::binary);

	fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;
	file.close();

	return fsize;
}


class myChip {
public:
	bool drawFlag = true;
	void loadGame(const char * c) {
		std::ifstream is;
		is.open("C:/Users/Ri03/source/repos/Chip-8-Emulator/x64/Debug/test.ch8", std::ios::binary);
		is.read((char *)(memory + 0x200), fileSize("C:/Users/Ri03/source/repos/Chip-8-Emulator/x64/Debug/test.ch8"));
		is.close();
	}
	void push(int c) {
		std::cout << "-" << (int)c << "-";
		key[sp] = c;
		sp++;
	}
	int pop() {
		int c = key[sp - 1];
		std::cout <<  "poping-" << (int)key[sp - 1] << "-";
		key[sp - 1] = 0;
		sp--;
		return c;
	}
	void emulateCycle() {
		opcode = (memory[pc] << 8);
		opcode |= memory[pc + 1];
		int x = opcode & 0x0F00 >> 8;
		printf("%04x", opcode);
		std::cout << "    " << pc << std::endl;
		switch (opcode >> 12) {
		printf("%04x", opcode >> 12);
		case 0:
			if ((opcode & 0x00FF) == 0xEE) {
				pc = pop();
				std::cout << pc << " new pc" << std::endl;
			}
			else if((opcode & 0x00F0) == 0xE0) {
				for (int i = 0; i < 64; i++) {
					for (int i2 = 0; i2 < 32; i2++) {
						gfx[i][i2] = false;
					}
				}
			}
			else {
				//I = opcode & 0x0FFF;
			}
			break;
			//Jumps to a memory address
		case 1:
			pc = opcode & 0X0FFF - 2;
			break;
			//Calls another memory address that can be returned from
		case 2:
			push(pc);
			pc = opcode & 0x0FFF - 2;
			break;
			//checks if a V var and NN are equal are equal
		case 3:
			if (V[opcode >> 8 & 0x0F] == opcode & 0x00FF) pc += 2;
			break;
			//checks if 2 V vars are not equal
		case 4:
			if (V[(opcode >> 8) & 0x0F] != opcode & 0x00FF) pc += 2;
			break;

		case 5:
			if (V[opcode & 0x0F00 >> 8] == V[opcode & 0x00F0 >> 8]) pc += 2;
			break;
		case 6:
			V[opcode & 0x0F00 >> 8] = opcode & 0x00FF
			break;
		case 7:
			V[opcode & 0x0F00 >> 8] += opcode & 0x00FF;
			break;
		case 8:
			switch (opcode & 0x000F) {
			case 0:
				V[opcode & 0x0F00 >> 8] = V[opcode & 0x00F0 >> 4];
				break;
			case 1:
				V[opcode & 0x0F00 >> 8] |= V[opcode & 0x00F0 >> 4];
				break;
			case 2:
				V[opcode & 0x0F00 >> 8] &= V[opcode & 0x00F0 >> 4];
				break;
			case 3:
				V[opcode & 0x0F00 >> 8] = pow(V[opcode & 0x0F00 >> 8], V[opcode & 0x00F0 >> 4]);
				break;
			case 4:
				V[opcode & 0x0F00 >> 8] += V[opcode & 0x00F0 >> 4];
				break;
			case 5:
				V[opcode & 0x0F00 >> 8] -= V[opcode & 0x00FF >> 4];
				break;
			case 6:
				V[15] = V[opcode & 0x0F00 >> 8] % 2;
				V[opcode & 0x0F00 >> 8] >>= 1;
				break;
			case 7:
				V[opcode & 0x0F00 >> 8] = V[opcode & 0x0F00 >> 8] - V[opcode & 0x00FF >> 4];
				break;
			case 0xE:
				V[15] = V[opcode & 0x0F00 >> 8] >> 7;
				V[opcode & 0x0F00 >> 8] <<= 1;
				break;
			}
			break;
		case 9:
			if (V[(opcode >> 8) & 0x0F] == V[opcode & 0x00F0] >> 4) pc += 2;
		case 0xA:
			I = opcode & 0x0FFF;
			break;
		case 0xB:
			pc = V[0] + opcode & 0x0FFF;
			break;
		case 0xC:
			V[opcode & 0x0F00 >> 8] = opcode & 0x00FF & (rand() % 255);
			break;
		case 0xD:
			// X V[opcode & 0x0F00 >> 8]  Y V[opcode & 0x00FF >> 4]
		{
			int height = opcode & 0x000F;
			int y = opcode & 0x00F0 >> 4;
			for (int i = 0; i < height; i++) {
				for (int i2 = 0; i2 < 8; i2++) {
					gfx[i2 + V[x]][i + V[y]] = ((memory[I + i] >> i2) & 1 == 1);
				}
			}
		}
		case 0xE:
			switch (opcode & 0x00FF) {

			case 0xA1:
			{
				SDL_PollEvent(&eventS);
				const char * c = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)eventS.type));
				char c2 = *c;
				if (V[x] == c2) {
					pc += 2;
				}
				break;
			}
			case 0x9E:
			{
				SDL_PollEvent(&eventS);
				const char * c = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)eventS.type));
				char c2 = *c;
				if (V[x] != c2) pc += 2;
				break;
			}
			}
		case 0xF:
			switch (opcode & 0x00FF) {
			case 7:
				V[opcode & 0x0F00 >> 8] = delay_timer;
				break;
			case 0xA:
			{
				do {
					SDL_PollEvent(&eventS);
				} while (eventS.type == NULL);
				SDL_Keycode key = SDL_GetKeyFromScancode((SDL_Scancode)eventS.type);
				const char * c = SDL_GetKeyName(key);
				V[x] = *c;
				break;
			}
			case 0x15:
				delay_timer = V[x];
				break;
			case 0x1E:
				I += V[x];
				break;
			case 0x29:
			{
				char c = memory[V[x]];
				I = memory[c * 5];
				break;
			}
			case 0x33:
				memory[I] = V[x] / 100;
				memory[I + 1] = (V[x] / 10) % 10;
				memory[I + 2] = (V[x] % 100) % 10;
				break;
			case 0x55:
				for (int i = 0; i < x; i++) {
					memory[I + i] = V[i];
				}
				break;
			case 0x65:
				for (int i = 0; i < x; i++) {
					V[i] = memory[I + i];
				}
				break;
			}
		}
		if (delay_timer > 0)
			--delay_timer;

		if (sound_timer > 0)
		{
			if (sound_timer == 1)
				printf("BEEP!\n");
			--sound_timer;
		}
		pc += 2;
	}

};
int main(int argc, char **argv) {
	myChip myChip8;
	init();
	myChip8.loadGame("test.ch");
	for (;;) {
		myChip8.emulateCycle();
		if (myChip8.drawFlag)
			drawGraphics();
	}
	return 0;
}
