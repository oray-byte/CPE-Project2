#include "avr/io.h"
#include "avr/interrupt.h"

//R0    G0    B0,   R1    G1    B1,   R2    G2    B2,   R3    G3    B3,   R4    G4    B4,   R5    G5    B5,   R6    G6    B6,   R7    G7    B7,   R8    G8    B8,   R9    G9    B9
unsigned char initValues[] = {0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10};
unsigned char neoPixels[30];

/*
 * -Iterates through neoPixels array
 * -Calls sendPixels()
 */
void updatePixels();

/*
 * -Sends pixels values to pixels
 * -Calls nextBit()
 */
void sendPixels(unsigned char R, unsigned char G, unsigned char B);

/*
 * -Initialize neopixel values with values in initValues array
 */
void initNeo();

/*
 * -Empty values in neopixels
 * -Clears neopixels
 */
void clearPixels();

/*
 * -Creates strobe light by calling allBlue(), allGreen(), and allRed() while clearing in between
 */
void strobe();

/*
 * -Plays a sound at 1 kHz
 */
void emitSound();

/*
 * -Helps emitSound() create a 500 us square wave
 */
void soundDelay();

/*
 * -Handles button presses.
 * -Returns 1 if pressed
 * -Returns 0 if not pressed
 */
int buttonPush();

/*
 * -Delay that helps creates stable button presses
 * -Uses timer1 to not interfere with timer0
 * -length: Control how long the delay is
 */
void buttonDelay(int length);

int main(void) {
	DDRB |= (1<<0);
	PORTB &= 0;
	DDRD &= 0b11101111; // Button on PD4
	DDRC |= 0b10000000; // Delete
	PORTD |= 0b00010000; // Pull-up resister
	DDRC |= 0b01000000; // Speaker on PC6
	int timer = 0;

	clearPixels(); // If pixels were not cleared before flashing
	
	while(1) {
		int dimVal = 4;
		// 100ms delay x 20 = 2s delay
		for (int _i = 0; _i < 20; _i++) {
			partDim(dimVal); // Responsible for increasing white brightness
			
			if (dimVal < 64) // Ensure value does not become greater than 64
			{
				dimVal += 3;
			}
			
			for (int _j = 0; _j < 10; _j++) {
				TCNT0 = 0xB1; // 10 ms delay for 10 times = 100ms delay
				TCCR0A = 0x00;
				TCCR0B = 0x05; // Prescaler 1024
				while ((TIFR0 & (1<<TOV0)) == 0)
				{
					timer = TCNT0; // Needed to 'pause' timer
					if (buttonPush()) // One beep to stop
					{
						while (buttonPush() != 1) // Two beeps to start
						{
							buttonDelay(0x00); // Needed to create stable button-pressing
						}
						TIFR0 = 0x01;
						TCNT0 = timer; // Needed to 'resume' timer
					}
					
				}
				TIFR0 = 0x01;
			}
		}

		// Reached after ~2 seconds
		while (buttonPush() != 1) { 
			strobe();
			for (int i = 0; i < 100; i++) // To waste time
			{
				emitSound();
			}
		}
	}
	
	return 0;
}

void buttonDelay(int length)
{
	TCNT1H = 0x00;
	TCNT1L = length;
	TCCR1A = 0x00;
	TCCR1B = 0x02;
	while ((TIFR1 & (1<<TOV1)) == 0);
	TIFR1 = 0x01;
}

int buttonPush() {
	if (PIND & (1<<4)) {
		TCNT1H = 0x00;
		TCNT1L = 0x91;
		TCCR1A = 0x00;
		TCCR1B = 0x02;
		
		while ((TIFR1 & (1<<TOV1)) == 0) // Need this so the sound is played long enough to hear
		{
			emitSound();
		}
		TIFR1 = 0x01;
		
		buttonDelay(0xFD); // Make a more stable button input
		
		return 1;
	}
	else {
		return 0;
	}
}

void emitSound() {
	PORTC ^= 0b01000000; // Toggle PORTC.6
	soundDelay();
	PORTC ^= 0b01000000; // Toggle PORTC.6
	soundDelay();
}

void soundDelay() {
	TCNT0 = 0xC1; // 500us delay
	TCCR0A = 0x00;
	TCCR0B = 0x03; // Prescaler 64

	while ((TIFR0 & (1<<TOV0)) == 0);
	TIFR0 = 0x01;
}

void strobe() {
	allRed();
	updatePixels();
	delay();
	clearPixels();
	delay();
	allBlue();
	delay();
	clearPixels();
	delay();
	allGreen();
	delay();
	clearPixels();
	delay();
}

void delay() {
	TCNT0 = -500;
	
	TCCR0A = 0x00;
	TCCR0B = 0x05;
	
	while((TIFR0 & (1<<TOV0)) == 0);
	
}

void initNeo() {
	for(int i = 0; i < 30; i++){
		neoPixels[i] = initValues[i]; // Mirrors functionality of assembly
	}
	
	// updatePixels();
	
}

void clearPixels() {
	for(int i = 0; i < 30; i++){
		neoPixels[i] = 0;
	}
	updatePixels();
	
}

void updatePixels() {
	
	for(int i = 0; i < 10; i++){
		unsigned char R = neoPixels[3*i]; //Red
		unsigned char G = neoPixels[3*i + 1]; //Green
		unsigned char B = neoPixels[3*i + 2]; //Blue
		
		sendPixels(R, G, B);
	}
}

void sendPixels(unsigned char R, unsigned char G, unsigned char B) {
	nextBit(R);
	nextBit(G);
	nextBit(B);
}

void nextBit(unsigned char color) {
	for(int i = 7; i >= 0; i--)
	{
		if(color & (1 << i)) // Check if the i'th bit is set using bitmasking
		{
			PORTB = 0x01;
			TCNT0 = -14;
			
			TCCR0A = 0x00;
			TCCR0B = 0x01;
			
			while((TIFR0 & (1<<TOV0)) == 0); // Wait until overflow flag is set (14 clock cycles)
			
			PORTB = 0x00; // Set PORTb low
			
			// Initialize the timer in normal mode with no prescaling
			TCCR0A = 0x00;
			TCCR0B = 0x01;
			
			TIFR0 = 1 << TOV0;
			
		}
		else
		{
			PORTB = 0x01; // Dr. Stanley told us about how this instruction was required for this to work
			PORTB = 0x00;
			
			// Initialize the timer with no prescaling in normal mode
			TCNT0 = -14;
			TCCR0A = 0x00;
			TCCR0B = 0x01;
			
			while((TIFR0 & (1 << TOV0)) == 0); // Run until overflow
			
			TCCR0A = 0x00;
			TCCR0B = 0x00;
			
			TIFR0 = 1 << TOV0;
			
		}
		
	}
	
}

void partDim(int x) {
	for(int i = 0; i < 10; i++){
		sendPixels(x, x, x);
	}
	delay();
}

void allGreen() {
	for(int i = 0; i < 10; i++){
		sendPixels(64, 0, 0);
	}
	updatePixels();
}

void allRed() {
	for(int i = 0; i < 10; i++){
		sendPixels(0, 64, 0);
	}
	updatePixels();
}

void allBlue() {
	for(int i = 0; i < 10; i++){
		sendPixels(0,0,64);
	}
	updatePixels();
}
