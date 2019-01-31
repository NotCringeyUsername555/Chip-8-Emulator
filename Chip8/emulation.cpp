#include "SDL.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <bitset>
#include <iostream>
#include <chrono>
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
unsigned short sp = 0;
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
	window = SDL_CreateWindow("Chip8", 100, 100, 640, 380, 0);
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
	for (int i = 0; i < 16; i++) {
		V[i] = 0;
	}
	for (int i = 0x200; i < 4096; i++) {
		//std::cout << memory[i] << " ";
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
char convertKeyCodeToChar(SDL_Keycode * kc) {
	switch (*kc) {
	case SDLK_0:
		return '0';
		break;
	case SDLK_1:
		return '1';
		break;
	case SDLK_2:
		return '2';
		break;
	case SDLK_3:
		return '3';
		break;
	case SDLK_4:
		return '4';
		break;
	case SDLK_5:
		return '5';
		break;
	case SDLK_6:
		return '6';
		break;
	case SDLK_7:
		return '7';
		break;
	case SDLK_8:
		return '8';
		break;
	case SDLK_9:
		return '9';
		break;
	case SDLK_a:
		return 'a';
		break;
	case SDLK_b:
		return 'b';
		break;
	case SDLK_c:
		return 'c';
		break;
	case SDLK_d:
		return 'd';
		break;
	case SDLK_e:
		return 'e';
		break;
	case SDLK_f:
		return 'f';
		break;
	}

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
		//std::cout << c;
		key[sp++] = c;
	}
	int pop() {
		int c = key[--sp];
		key[sp] = 0;
		return c;
	}
	void emulateCycle() {
		opcode = (memory[pc] << 8);
		opcode |= memory[pc + 1];
		//printf("%.4x\n", opcode);
		int x = (opcode & 0x0F00) >> 8;
		int y = (opcode & 0x00F0) >> 4;
		//printf("%04x", opcode);
		//std::cout << "    " << pc << std::endl;
		switch (opcode >> 12) {
			//printf("%04x", opcode >> 12);
		case 0:
			//returns
			if ((opcode & 0x00FF) == 0xEE) {
				pc = pop();
				//	std::cout << pc << " new pc" << std::endl;
			}
			//clears the screen
			else if ((opcode & 0x00F0) == 0xE0) {
				for (int i = 0; i < 64; i++) {
					for (int i2 = 0; i2 < 32; i2++) {
						gfx[i][i2] = false;
					}
				}
				drawFlag = true;
			}
			else {
				//does a bunch of shit I dont understand
			}
			break;
			//Jumps to a memory address
		case 1:
			pc = (opcode & 0X0FFF) - 2;
			break;
			//Calls another memory address that can be returned from
		case 2:
			push(pc);
			pc = (opcode & 0x0FFF) - 2;
			break;
			//checks if a V var and NN are equal are equal
		case 3:
			if (V[x] == (opcode & 0x00FF))
				pc += 2;
			break;
			//checks if 2 V vars are equal
		case 4:
			if (V[x] != (opcode & 0x00FF)) pc += 2;
			break;
		case 5:
			if (V[x] == V[y]) pc += 2;
			break;
		case 6:
			V[x] = (opcode & 0x00FF);
			break;
		case 7:
			V[x] += (opcode & 0x00FF);
			break;
		case 8:
			switch (opcode & 0x000F) {
			case 0:
				V[x] = V[y];
				break;
			case 1:
				V[x] |= V[y];
				break;
			case 2:
				V[x] &= V[y];
				break;
			case 3:
				V[x] = V[x] ^ V[y];
				break;
			case 4:
				V[x] += V[y];
				V[0xf] = (255 < V[x] || V[x] < 0);
				break;
			case 5:
				if (V[x] > V[y]) {
					V[0xf] = 1;
				}
				else V[0xf] = 0;
				V[x] -= V[y];
				//std::cout << "this is the value: " << (int)V[0xf] << std::endl;
				break;
			case 6:
				V[0xF] = V[x] % 2;
				V[x] /= 2;
				break;
			case 7:
				V[0xF] = 0;
				if (V[y] > V[x]) V[0xF] = 1;
				V[x] = V[x] - V[y];
				break;
			case 0xE:
				V[15] = V[x] >> 7;
				V[x] *= 2;
				break;
			}
			break;
		case 9:
			if (V[x] != V[y]) pc += 2;
		case 0xA:
			I = opcode & 0x0FFF;
			break;
		case 0xB:
			pc = V[0] + (opcode & 0x0FFF);
			break;
		case 0xC:
			V[x] = (opcode & 0x00FF) & (rand() % 255);
			break;
		case 0xD:
			// X V[opcode & 0x0F00 >> 8]  Y V[opcode & 0x00FF >> 4]
		{

			for (int i = 0; i < 64; i++) {
				for (int i2 = 0; i2 < 32; i2++) {
					gfx[i][i2] = false;
				}
			}
			int height = opcode & 0x000F;
			V[0xF] = 0;
			for (int i = 0; i < height; i++) {
				for (int i2 = 0; i2 < 8; i2++) {
					bool b = gfx[(8 - i2) + V[x]][i + V[y]];
					gfx[(8 - i2) + V[x]][i + V[y]] = ((memory[I + i] >> i2) & 1 == 1);
					if (b != gfx[(8 - i2) + V[x]][i + V[y]]) V[0xF] = 1;
				}
			}
			drawFlag = true;
		}
		case 0xE:
			switch (opcode & 0x00FF) {

			case 0xA1:
			{
				SDL_PollEvent(&eventS);
				if (eventS.type == SDL_KEYDOWN) {
					std::cout << (int)convertKeyCodeToChar(&eventS.key.keysym.sym) << " =/= " <<(int) V[x];
					if (V[x] == convertKeyCodeToChar(&eventS.key.keysym.sym)) pc += 2;
				}
				break;
			}
			case 0x9E:
			{
				SDL_PollEvent(&eventS);
				if (eventS.type == SDL_KEYDOWN) {
					std::cout << (int)convertKeyCodeToChar(&eventS.key.keysym.sym) << " = " << (int)V[x];
					if (V[x] != convertKeyCodeToChar(&eventS.key.keysym.sym)) pc += 2;
				}
				break;
			}
			}
		case 0xF:
			switch (opcode & 0x00FF) {
			case 7:
				V[x] = delay_timer;
				break;
			case 0xA:
			{
				SDL_StartTextInput();
				do{
					SDL_PollEvent(&eventS);
					if (eventS.type == SDL_KEYDOWN) {
						V[x] = convertKeyCodeToChar(&eventS.key.keysym.sym);
					}
				} while (eventS.type != SDL_KEYDOWN);
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
				int c = V[x];
				I = c * 5;
				std::cout << c * 5;
				break;
			}
			case 0x33:
				memory[I] = V[x] / 100;
				//std::cout << "\n" << "memory[I] = " << (int)memory[I];
				memory[I + 1] = (V[x] / 10) % 10;
				memory[I + 2] = (V[x] % 100) % 10;
				break;
			case 0x55:
				for (int i = 0; i < x + 1; i++) {
					memory[I + i] = V[i];
				}
				break;
			case 0x65:
				for (int i = 0; i < x + 1; i++) {
					//std::cout << "\n" << "Memory[I + " << i << "] = " << (int)memory[I + i];
					V[i] = (int)memory[I + i];
					//std::cout << "V[" << i + I << "] = " << (int)memory[I + i];
				}
				break;
			default:
				break;
			}
		}
		if (delay_timer > 0)
			--delay_timer;

		if (sound_timer > 0)
		{
			if (sound_timer == 1)
				//printf("BEEP!\n");
			--sound_timer;
		}
		pc += 2;
	}

};
int main(int argc, char **argv) {
	myChip myChip8;
	init();
	for (int i = 0; i < 64; i++) {
		for (int i2 = 0; i2 < 35; i2++) {
			gfx[i][i2] = false;
		}
	}
	myChip8.loadGame("test.ch");
	std::chrono::milliseconds millis = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
	for (;;) {
		myChip8.emulateCycle();
		if (myChip8.drawFlag) {
			drawGraphics();
			myChip8.drawFlag = false;
		}
	}
	std::chrono::milliseconds newMillis = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
	std::chrono::milliseconds i = newMillis - millis;
	i = std::chrono::milliseconds(60) - i;
	SDL_Delay(i.count());
	return 0;
}