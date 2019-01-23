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
	void emulateCycle() {
		opcode = memory[pc] << 8 | memory[pc + 1];
		if (opcode & 0x0FFF == 0) {
			I = opcode & 0x0FFF;
			pc += 2;
		}
		else if (opcode & 0x0FFF == 0) {

		}
		else if (opcode == 0x00E0) {
			for (char c : gfx) {
				c = 0;
			}
		}
		else if (opcode == 0x00EE) {

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