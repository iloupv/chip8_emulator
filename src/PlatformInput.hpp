#include <cstdint>

class PlatformInput
{
public:
	PlatformInput();
	bool quit;
	bool pause;
	bool nextInstruction;
	bool debugger;
	bool reset;
	bool saveState;
	bool loadState;
	bool ProcessInput(uint8_t* keys);
};
