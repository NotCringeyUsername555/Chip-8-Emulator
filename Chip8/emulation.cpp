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
unsigned char key[16];
std::ifstream fs;
SDL_Window * window;

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
		case 9:
			if (V[opcode >> 8 & 0x0F] == V[opcode & 0x00F0] >> 4) pc += 4;
			else pc += 2;
		case 0xA:
			I = opcode & 0x0FFF;
			break;
		case 0xB:
			pc = V[0] +opcode & 0x0FFF;
			break;
		case 0xC:
			V[opcode & 0x0F00 >> 8] = opcode & 0x00FF & (rand() % 255);
			break;
		case 0xD:

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