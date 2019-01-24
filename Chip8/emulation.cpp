#include "SDL.h"
#include <fstream>
unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16];
unsigned short I;
unsigned short pc;
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
		fs.open("test.ch8");
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
			if (V[opcode >> 8 & 0x0F] != opcode & 0x00FF) pc += 4;
			else pc += 2;
			break;
		case 4:
			if (V[opcode >> 8 & 0x0F] == opcode & 0x00FF) pc += 4;
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
		}
	}
};
int main(int argc, char **argv) {
	myChip myChip8;
	myChip8.emulateCycle();
	/*init();
	myChip8.loadGame("test.ch");
	for (;;) {
		myChip8.emulateCycle();
		if (myChip8.drawFlag)
			drawGraphics();
	}
	return 0;*/
	return 0;
}