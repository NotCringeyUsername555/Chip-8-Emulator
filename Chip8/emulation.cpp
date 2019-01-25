#include "SDL.h"
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16];
unsigned short I;
unsigned short pc = 0x200;
unsigned char gfx[64 * 32];
unsigned char delay_timer;
unsigned char sound_timer;
unsigned short sp;
SDL_Event eventS;
unsigned char key[16];
std::ifstream fs;
SDL_Window * window;
unsigned char chip8_fontset[80] =
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

}
void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Chip8", 100, 100, 650, 320, 0);
}
class myChip {
public:
	bool drawFlag;
	void loadGame(const char * c) {
		FILE *file;
		char *code = (char *)malloc(1000 * sizeof(char));
		file = fopen("test.ch8", "rb");
		int i = 0;
		do
		{
			memory[0x200 + i] = fgetc(file);
			i++;
		} while (*code != EOF);
	}
	void push(char c) {
		key[sp] = c;
		sp++;
	}
	char pop() {
		char c = key[sp - 1];
		key[sp - 1] = 0;
		return c;
	}
	void emulateCycle() {
		opcode = memory[pc] << 8 | memory[pc + 1];
		switch (opcode >> 12) {
		case 0:
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 1:
			pc = opcode & 0X0FFF;
			break;
		case 2:
			push(pc);
			pc = opcode & 0x0FFF;
			break;
		case 3:
			if (V[opcode >> 8 & 0x0F] != V[opcode & 0x00FF]) pc += 4;
			else pc += 2;
			break;
		case 4:
			if (V[opcode >> 8 & 0x0F] == V[opcode & 0x00FF]) pc += 4;
			else pc += 2;
			break;
		case 5:
			if (V[opcode & 0x0F00 >> 8] != V[opcode & 0x00F0 >> 8]) pc += 2;
			pc += 2;
			break;
		case 6:
			if (V[opcode & 0x0F00 >> 8] != V[opcode & 0x00F0 >> 8]) pc += 2;
			pc += 2;
			break;
		case 7:
			V[opcode & 0x0F00 >> 8] += V[opcode & 0x00FF];
			pc += 2;
			break;
		case 8:
			switch (opcode & 0x000F) {
			case 0:
				V[opcode & 0x0F00 >> 8] = V[opcode & 0x00F0 >> 4];
				break;
			case 1:
				V[opcode & 0x0F00 >> 8] |= V[opcode & 0x00F0 >> 4];
				pc += 2;
				break;
			case 2:
				V[opcode & 0x0F00 >> 8] &= V[opcode & 0x00F0 >> 4];
				pc += 2;
				break;
			case 3:
				V[opcode & 0x0F00 >> 8] = pow(V[opcode & 0x0F00 >> 8], V[opcode & 0x00F0 >> 4]);
				pc += 2;
				break;
			case 4:
				V[opcode & 0x0F00 >> 8] += V[opcode & 0x00F0 >> 4];
				pc += 2;
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
		case 9:
			if (V[opcode >> 8 & 0x0F] == V[opcode & 0x00F0] >> 4) pc += 4;
			else pc += 2;
		case 0xA:
			I = opcode & 0x0FFF;
			break;
		case 0xB:
			pc = V[0] + opcode & 0x0FFF;
			break;
		case 0xC:
			V[opcode & 0x0F00 >> 8] = opcode & 0x00FF & (rand() % 255);
			pc += 2;
			break;
		case 0xD:
			// X V[opcode & 0x0F00 >> 8]  Y V[opcode & 0x00FF >> 4]
			int height = opcode & 0x000F;
			int x = opcode & 0x0F00 >> 8;
			int y = opcode & 0x00F0 >> 4;
			for (int i = V[y] * 64 + V[x]; i < V[y] * 64 + V[x] + (height * 64); i += 64) {
				gfx[i] = *((char *)I);
			}
			pc += 2;
		case 0xE:
			int x = opcode & 0x0F00 >> 8;
			char c = V[x];
			switch (opcode & 0x00FF) {
			case 0xA1:
				SDL_PollEvent(&eventS);
				int x = opcode & 0x0F00 >> 8;
				const char * c = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)eventS.type)));
				char c2 = *c;
				if (V[x] == c2) {
					pc += 4;
				}
				else
					pc += 2;
				break;
			case 0x9E:
				SDL_PollEvent(&eventS);
				int x = opcode & 0x0F00 >> 8;
				const char * c = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)eventS.type)));
				char c2 = *c;
				if (V[x] != c2) pc += 4;
				else pc += 2;
				break;
			}
		case 0xF:
			switch (opcode & 0x00FF) {
				case 7:
					V[opcode & 0x0F00 >> 8] = delay_timer;
					pc += 2;
					break;
				case 0xA:
				{
					int x = opcode & 0x0F00 >> 8;
					do {
						SDL_PollEvent(&eventS);
					} while (eventS.type == NULL);
					SDL_Keycode key = SDL_GetKeyFromScancode((SDL_Scancode)eventS.type);
					const char * c = SDL_GetKeyName(key);
					V[x] = *c;
					break;
				}
				case 0x15:
					int x = opcode & 0x0F00 >> 8;
					delay_timer = V[x];
					break;
				case 0x1E:
					int x = opcode & 0x0F00 >> 8;
					I += V[x];
					break;
				case 0x29:
					int x = opcode & 0x0F00 >> 8;
					char c = *((char *)V[x]);
					int loc = (int)c;
					I = (unsigned int)&chip8_fontset[loc * 5];
					break;
				//TODO: Implement 0x33
				case 0x55:


			}
		}
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