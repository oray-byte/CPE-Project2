
Skip to content
Pull requests
Issues
Marketplace
Explore
@oray-byte
oray-byte /
CPE-Project2
Public

Code
Issues
Pull requests
Actions
Projects
Wiki
Security
Insights

    Settings

CPE-Project2/Task2.c
@elloGarrett
elloGarrett Working strobe() and emitSound()
Latest commit eccbb0f 5 hours ago
History
1 contributor
237 lines (168 sloc) 4.01 KB
#include "avr/io.h"
#include "avr/interrupt.h"

//R0    G0    B0,   R1    G1    B1,   R2    G2    B2,   R3    G3    B3,   R4    G4    B4,   R5    G5    B5,   R6    G6    B6,   R7    G7    B7,   R8    G8    B8,   R9    G9    B9
unsigned char initValues[] = {0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10};
unsigned char neoPixels[30];


void updatePixels();

void sendPixels(unsigned char R, unsigned char G, unsigned char B);

void initNeo();

void clearPixels();

void sendZero();

void sendOne();

void strobe();

void startTimer();

void stopTimer();

void emitSound();


int main() {
	DDRB |= (1<<0);
	PORTB &= 0;
	DDRC |= 0b01000000; //Speaker on PD6
	
	initNeo();
	
	while(1) {
		emitSound();
		//strobe();
	}
	
	return 0;
}


void startTimer() {
	PORTB &= 0b00000000;
	
	// Put 7 clock cycle low timer here
	TCNT0 = -7;
	TCCR0A = 0x00;
	TCCR0B = 0x03;
	
	while((TIFR0 & (1 << TOV0)) == 0);
	
	TCCR0A = 0;
	TIFR0 = 0x1;
}

void stopTimer() {
	
}

void emitSound() {
	PORTC |= 0b01000000;
	soundDelay();	
	PORTC &= 0b00000000;
	soundDelay();
}

void soundDelay() {
	TCNT0 = 0xC1;
	TCCR0A = 0x00;
	TCCR0B = 0x03;

	while ((TIFR0 & (1<<TOV0)) == 0);
	TIFR0 = 0x01;
}

void strobe() {
	allRed();
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
	PORTB = 0x01;
	TCNT0 = -250;
	
	TCCR0A = 0x00;
	TCCR0B = 0x05;
	
	while((TIFR0 & (1<<TOV0)) == 0); // Wait until overflow flag is set (14 clock cycles)
	
	PORTB = 0x00; // Set PORTb low
	
	// Initialize the timer in normal mode with no prescaling
	TCCR0A = 0x00;
	TCCR0B = 0x01;
	
	TIFR0 = 1 << TOV0;
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

void sendOne() {
	PORTB |= 0b00000001;
	
	// Put 14 clock cycle high timer here
	TCNT0 = -14;
	TCCR0A = 0x00;
	TCCR0B = 0x01;
	
	while((TIFR0 & (1 << TOV0)) == 0);
	
	TCCR0A = 0;
	TIFR0 = 0x1; // Clear the timer

}

void allGreen() {
	for(int i = 0; i < 10; i++){
		sendPixels(64, 0, 0);
	}
}

void allRed() {
	for(int i = 0; i < 10; i++){
		sendPixels(0, 64, 0);
	}
}

void allBlue() {
	for(int i = 0; i < 10; i++){
		sendPixels(0,0,64);
	}
}

void sendZero() {
	PORTB &= 0b00000000;
	
	// Put 7 clock cycle low timer here
	TCNT0 = -7;
	TCCR0A = 0x00;
	TCCR0B = 0x01;
	
	while((TIFR0 & (1 << TOV0)) == 0);
	
	TCCR0A = 0;
	TIFR0 = 0x1;
}

    © 2021 GitHub, Inc.

    Terms
    Privacy
    Security
    Status
    Docs
    Contact GitHub
    Pricing
    API
    Training
    Blog
    About

Loading complete