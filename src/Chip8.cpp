#include "Chip8.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <random>
#include <iterator>
#include <algorithm>


void Chip8::Initialize(std::string rom)
{
	uint8_t sprites[16*5] =
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
	
	// Initialize PC
	pc = 0x200;
	spc = 0x200;
	
	// Initialize Index Register
	index = 0x0000;
	sindex = 0x0000;	
	// Initialize Registers, Stack and Keypad
	for(int i = 0; i<16; i++)
	{
		registers[i] = 0x00;
		sregisters[i] = 0x00;
		
		stack[i] = 0x0000;
		sstack[i] = 0x0000;
		
		keypad[i] = 0x00;
		skeypad[i] = 0x00;
	}
	
	// Initialize Stack Pointer	
	sp = 0x00;
	ssp = 0x00;
	
	// Initialize display	
	for(int i = 0; i < (64*32); i++)
	{
		display[i] = 0x00000000;
		sdisplay[i] = 0x00000000;
	}
	
	// Initialize Memory
	for(int i = 0; i < 4096; i++)
	{
		memory[i] = 0x00;
		smemory[i] = 0x00;
	}
	
	// Initialize opcode
	
	opcode = 0x0000;
	sopcode = 0x0000;

	// Load fonts into memory
	for (unsigned int i = 0; i < (16*5); ++i)
	{
		memory[0x050 + i] = sprites[i];
		smemory[0x050 + i] = sprites[i];
	}
	
	// Load ROM into memory
	
	FILE *romR = fopen(rom.c_str(), "rb");
	if(romR == NULL){
        	printf("No se ha podido localizar el archivo de entrada.\n");
		exit(0);
	}

	fseek(romR, 0, SEEK_END);
	long int romSize = ftell(romR);
	fseek(romR, 0, SEEK_SET);

	char *buffer = (char*) malloc(sizeof(char)*romSize);
	if(buffer == NULL){
		printf("No se ha podido cargar los datos del archivo de entrada.\n");
		exit(0);
	}
	fread(buffer, 1, romSize, romR);

	// Load data into memory
	for(int i = 0; i < romSize; i++){
		memory[0x200 + i] = buffer[i];
		smemory[0x200 + i] = buffer[i];
	}

	fclose(romR);
	free(buffer);
}

void Chip8::writeTerminalDebug(){
	std::system("clear");
	for(int i = 0; i<16 ;i++){
		printf("Register %d = %X \n", i, registers[i]);
	}
	printf("Index Register = %X\n", index);
	printf("Program Counter = %X\n", pc);
	printf("Opcode = %X\n", opcode);
	printf("Delay Timer = %X\n", delayTimer);
	printf("Sound Timer = %X\n", soundTimer);
	printf("Top of Stack = %X\n", stack[sp]);
	printf("Stack Pointer = %X\n", sp);
}

void Chip8::saveState(){
	std::copy(std::begin(memory), std::end(memory), std::begin(smemory));
	std::copy(std::begin(registers), std::end(registers), std::begin(sregisters));
	sindex = index;
	spc = pc;
	sdelayTimer = delayTimer;
	ssoundTimer = soundTimer;
	std::copy(std::begin(stack), std::end(stack), std::begin(sstack));
	ssp = sp;
	sopcode = opcode;
	std::copy(std::begin(keypad), std::end(keypad), std::begin(skeypad));
	std::copy(std::begin(display), std::end(display), std::begin(sdisplay));	
}

void Chip8::loadState(){
	std::copy(std::begin(smemory), std::end(smemory), std::begin(memory));
	std::copy(std::begin(sregisters), std::end(sregisters), std::begin(registers));
	index = sindex;
	pc = spc;
	delayTimer = sdelayTimer;
	soundTimer = ssoundTimer;
	std::copy(std::begin(sstack), std::end(sstack), std::begin(stack));
	sp = ssp;
	opcode = sopcode;
	std::copy(std::begin(skeypad), std::end(skeypad), std::begin(keypad));
	std::copy(std::begin(sdisplay), std::end(sdisplay), std::begin(display));	
}

void Chip8::Cycle()
{
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	pc += 2;

	DecodeAndExecute();
}

void Chip8::UpdateTimers()
{
	if (delayTimer > 0)
	{
		--delayTimer;
	}

	if (soundTimer > 0)
	{	
		playSound = true;
		--soundTimer;
	}
	else{	
		playSound = false;
	}
}

void Chip8::DecodeAndExecute()
{	

	switch(opcode & 0xF000u){
		case 0x0000u:
			switch(opcode & 0x00FFu){

					case 0x00E0u:
						OP_00E0();
						break;

					case 0x00EEu:
						OP_00EE();		
						break;
			}
			break;

		case 0x1000u:
			OP_1nnn();
			break;

		case 0x2000u:
			OP_2nnn();
			break;

		case 0x3000u:
			OP_3xkk();
			break;

		case 0x4000u:
			OP_4xkk();
			break;

		case 0x5000u:
			OP_5xy0();
			break;

		case 0x6000u:
			OP_6xkk();
			break;

		case 0x7000u:
			OP_7xkk();
			break;


		case 0x8000u:
			switch(opcode & 0x000Fu){

				case 0x0000u:
					OP_8xy0();
					break;

				case 0x0001u:
					OP_8xy1();
					break;

				case 0x0002u:
					OP_8xy2();
					break;

				case 0x0003u:
					OP_8xy3();
					break;

				case 0x0004u:
					OP_8xy4();
					break;

				case 0x0005u:
					OP_8xy5();
					break;

				case 0x0006u:
					OP_8xy6();
					break;

				case 0x0007u:
					OP_8xy7();
					break;

				case 0x000Eu:
					OP_8xyE();
					break;
			}
			break;
		
		case 0x9000u:
			OP_9xy0();
			break;

		case 0xA000u:
			OP_Annn();
			break;

		case 0xB000u:
			OP_Bnnn();
			break;

		case 0xC000u:
			OP_Cxkk();
			break;

		case 0xD000u:
			OP_Dxyn();
			break;

		case 0xE000u:
			switch(opcode & 0x000Fu){

				case 0x000Eu:
					OP_Ex9E();
					break;

				case 0x0001u:
					OP_ExA1();
					break;
			}

			break;

		case 0xF000u:
		    	switch(opcode & 0x00FFu){
					case 0x0007u:
						OP_Fx07();
						break;

					case 0x000Au:
						OP_Fx0A();
						break;

					case 0x0015u:
						OP_Fx15();
						break;

					case 0x0018u:
						OP_Fx18();
						break;

					case 0x001Eu:
						OP_Fx1E();
						break;

					case 0x0029u:
						OP_Fx29();
						break;

					case 0x0033u:
						OP_Fx33();
						break;

					case 0x0055u:
						OP_Fx55();
						break;

					case 0x0065u:
						OP_Fx65();
						break;
			}
        break;

        default:
            OP_NULL();
            break;
	}

		
}
void Chip8::OP_NULL()
{
	printf("Operación no identificada. Se cerrará el programa.");
	exit(0);
}

void Chip8::OP_00E0()
{
	for(int i = 0; i < 32*64; i++)
	{
		display[i] = 0;
	}
}

void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];
}

void Chip8::OP_1nnn()
{
	pc = opcode & 0x0FFFu;
}

void Chip8::OP_2nnn()
{
	stack[sp] = pc;
	++sp;
	pc = opcode & 0x0FFFu;
}

void Chip8::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (registers[Vx] == (opcode & 0x00FFu))
	{
		pc += 2;
	}
}

void Chip8::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (registers[Vx] != (opcode & 0x00FFu))
	{
		pc += 2;
	}
}

void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = opcode & 0x00FFu;
}

void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] += opcode & 0x00FFu;
}

void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t acc = registers[Vx] + registers[Vy];

	if (acc > 255u)
	{
		registers[0xF] = 1;
	}
	
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = acc;
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[15] = (registers[Vx] & 0x80u) >> 7u;
	registers[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_Annn()
{
	index = opcode & 0x0FFFu;
}

void Chip8::OP_Bnnn()
{
	pc = registers[0] + (opcode & 0x0FFFu);
}

void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;	
	registers[Vx] = (rand() % 255u) & (opcode & 0x00FFu);
}

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;
	uint8_t xPos = registers[Vx] % 64;
	uint8_t yPos = registers[Vy] % 32;
	registers[0xF] = 0;

	for (int row = 0; row < height; ++row)
	{
		uint8_t spriteRow = memory[index + row];

		for (int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteRow & (0x80u >> col);
			uint32_t* screenPixel = &display[(yPos + row) * 64 + (xPos + col)];

			if (spritePixel)
			{

				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[registers[Vx]])
	{
		pc += 2;
	}
}

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;


	if (!keypad[registers[Vx]])
	{
		pc += 2;
	}
}

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	bool pressed = false;
	
	for(int i = 0; i < 16; i++)
	{
		if(keypad[i] && !pressed)
		{
			pressed = true;
			registers[Vx] = i;
		}
	}
	
	if(!pressed)
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delayTimer = registers[Vx];
}

void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += registers[Vx];
}

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index = 0x050 + (5 * registers[Vx]);
}

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t result = registers[Vx];

	memory[index + 2] = result % 10;
	result /= 10;

	memory[index + 1] = result % 10;
	result /= 10;

	memory[index] = result % 10;
}

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (int i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}

void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (int i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
}
