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
				switch (eventS.type) {
				case SDL_SCANCODE_0:
					if (c == '0') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_1:
					if (c == '1') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_2:
					if (c == '2') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_3:
					if (c == '3') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_4:
					if (c == '4') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_5:
					if (c == '5') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_6:
					if (c == '6') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_7:
					if (c == '7') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_8:
					if (c == '8') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_9:
					if (c == '9') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_A:
					if (c == 'A') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_B:
					if (c == 'B') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_C:
					if (c == 'C') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_D:
					if (c == 'D') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_E:
					if (c == 'E') pc += 4;
					else pc += 2;
					break;
				case SDL_SCANCODE_F:
					if (c == 'F') pc += 4;
					else pc += 2;
					break;

				}

				break;
			case 0x9E:
				switch (opcode & 0x00FF) {
				case 0xA1:
					SDL_PollEvent(&eventS);
					switch (eventS.type) {
					case SDL_SCANCODE_0:
						if (c != '0') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_1:
						if (c != '1') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_2:
						if (c != '2') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_3:
						if (c != '3') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_4:
						if (c != '4') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_5:
						if (c != '5') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_6:
						if (c != '6') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_7:
						if (c != '7') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_8:
						if (c != '8') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_9:
						if (c != '9') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_A:
						if (c != 'A') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_B:
						if (c != 'B') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_C:
						if (c != 'C') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_D:
						if (c != 'D') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_E:
						if (c != 'E') pc += 4;
						else pc += 2;
						break;
					case SDL_SCANCODE_F:
						if (c != 'F') pc += 4;
						else pc += 2;
						break;
					}
				break;
				case 0xF:
					switch (opcode & 0x00FF) {
					case 7:
						V[opcode & 0x0F00 >> 8] = delay_timer;
						pc += 2;
						break;
					case 0xA:
						int x = opcode & 0x0F00 >> 8;

						break;
					}
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