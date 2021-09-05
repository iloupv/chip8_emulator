#include "Chip8.hpp"
#include "PlatformDisplay.hpp"
#include "PlatformSound.hpp"
#include "PlatformInput.hpp"
#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Uso: " << argv[0] << " |Ciclos por Segundo| |Nombre del Archivo|\n";
		exit(1);
	}

	int cyclesPerSecond = std::stoi(argv[1]);
	std::string romFilename = argv[2];
	
	bool reset = true;
	
	while(reset){
		
		reset = false;
		PlatformDisplay platform("Emulador CHIP-8", 64 * 20, 32 * 20, 64, 32);
		
		SDL_Init(SDL_INIT_AUDIO);
		PlatformSound sound;
		
		PlatformInput input;

		Chip8 chip8;
		chip8.Initialize(romFilename);

		int displayPitch = sizeof(chip8.display[0]) * 64;
		
		float waitTimePer60 = (float) 1000/ (float) 60;
		float waitTimePerCycle = (float) 1000/ (float) cyclesPerSecond;
		float accumulatedTime = 0;
		float passedTime = 0;
		float passedTimeSound = 0;
		auto lastCycleTime = std::chrono::high_resolution_clock::now();
		
		auto currentTimeSound = std::chrono::high_resolution_clock::now();;
		auto currentTimeSound2 = std::chrono::high_resolution_clock::now();;
		
		// Main loop
		while (!input.quit)
		{
			input.ProcessInput(chip8.keypad);
			
			if(input.saveState){
				chip8.saveState();
				input.saveState = false;
			}
			
			if(input.loadState){
				chip8.loadState();
				input.loadState = false;
			}
			
			if(input.debugger){
				chip8.writeTerminalDebug();
			}
			
			while(input.pause){
				input.ProcessInput(chip8.keypad);
				
				if(input.reset){
					reset = true;
					break;
				}
				
				else if(input.saveState){
					chip8.saveState();
					input.saveState = false;
				}
				
				else if(input.loadState){
					chip8.loadState();
					input.loadState = false;
					break;
				}
				
				else if(input.nextInstruction || input.quit){
					chip8.writeTerminalDebug();
					input.nextInstruction = false;
					break;
				}		
			}
			
			
			auto currentTime = std::chrono::high_resolution_clock::now();
			passedTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
			accumulatedTime += passedTime;

			if (passedTime > waitTimePerCycle)
			{	
				// CPU Cycle	
				lastCycleTime = currentTime;
				chip8.Cycle();
				platform.Update(chip8.display, displayPitch);
				
				//Timer Update
				if (accumulatedTime > waitTimePer60)
				{
					chip8.UpdateTimers();
					accumulatedTime = 0;
				}
				
				// Sound
				if(chip8.playSound)
				{	
					currentTimeSound = std::chrono::high_resolution_clock::now();
					sound.play();	
				}
				
				else{
					currentTimeSound2 = std::chrono::high_resolution_clock::now();
					passedTimeSound = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTimeSound2 - currentTimeSound).count();
					if(passedTimeSound > 75)
					{
						sound.stop();
					}
				}
			}
			
			if(input.reset){
				reset = true;
				break;
			}
		}
	}
	return 0;
}
