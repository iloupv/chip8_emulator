#include <cstdint>
#include <random>

class Chip8
{
public:
	void Initialize(std::string rom);
	void writeTerminalDebug();
	void saveState();
	void loadState();
	void Cycle();
	void Fetch();
	void DecodeAndExecute();
	void UpdateTimers();
	
	// ACTUAL VARIABLES
	
	uint8_t memory[4096]{};
	
	uint8_t registers[16]{};
	uint16_t index{};
	
	uint16_t pc{};
	
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	
	uint16_t stack[16]{};
	uint8_t sp{};
	
	uint16_t opcode{};
	
	uint8_t keypad[16]{};
	
	uint32_t display[64 * 32]{};
	
	// SAVED VARIABLES
	
	uint8_t smemory[4096]{};
	
	uint8_t sregisters[16]{};
	uint16_t sindex{};
	
	uint16_t spc{};
	
	uint8_t sdelayTimer{};
	uint8_t ssoundTimer{};
	
	uint16_t sstack[16]{};
	uint8_t ssp{};
	
	uint16_t sopcode{};
	
	uint8_t skeypad[16]{};
	
	uint32_t sdisplay[64 * 32]{};
	
	bool playSound;
	
	// INSTRUCTIONS

	// Non identified instruction
	void OP_NULL();

	// CLS
	void OP_00E0();

	// RET
	void OP_00EE();

	// JP address
	void OP_1nnn();

	// CALL address
	void OP_2nnn();

	// SE Vx, byte
	void OP_3xkk();

	// SNE Vx, byte
	void OP_4xkk();

	// SE Vx, Vy
	void OP_5xy0();

	// LD Vx, byte
	void OP_6xkk();

	// ADD Vx, byte
	void OP_7xkk();

	// LD Vx, Vy
	void OP_8xy0();

	// OR Vx, Vy
	void OP_8xy1();

	// AND Vx, Vy
	void OP_8xy2();

	// XOR Vx, Vy
	void OP_8xy3();

	// ADD Vx, Vy
	void OP_8xy4();

	// SUB Vx, Vy
	void OP_8xy5();

	// SHR Vx
	void OP_8xy6();

	// SUBN Vx, Vy
	void OP_8xy7();

	// SHL Vx
	void OP_8xyE();

	// SNE Vx, Vy
	void OP_9xy0();

	// LD I, address
	void OP_Annn();

	// JP V0, address
	void OP_Bnnn();

	// RND Vx, byte
	void OP_Cxkk();

	// DRW Vx, Vy, height
	void OP_Dxyn();

	// SKP Vx
	void OP_Ex9E();

	// SKNP Vx
	void OP_ExA1();

	// LD Vx, DT
	void OP_Fx07();

	// LD Vx, K
	void OP_Fx0A();

	// LD DT, Vx
	void OP_Fx15();

	// LD ST, Vx
	void OP_Fx18();

	// ADD I, Vx
	void OP_Fx1E();

	// LD F, Vx
	void OP_Fx29();

	// LD B, Vx
	void OP_Fx33();

	// LD [I], Vx
	void OP_Fx55();

	// LD Vx, [I]
	void OP_Fx65();
};
