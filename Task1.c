#include "avr/io.h"
#include "avr/interrupt.h"

					//R0    G0    B0,   R1    G1    B1,   R2    G2    B2,   R3    G3    B3,   R4    G4    B4,   R5    G5    B5,   R6    G6    B6,   R7    G7    B7,   R8    G8    B8,   R9    G9    B9
unsigned char initValues[] = {0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10};
unsigned char neoPixels[30];


void updatePixels();

void sendPixels(unsigned char R, unsigned char G, unsigned char B);

void initNeo();

void clearPixels();

void byteLoop(unsigned char colorVal);

void sendZero();

void sendOne();


int main(){
	DDRB = 0x00;
	PORTB = 0;
	
	initNeo();
	
	return 0;
}

void initNeo(){
	for(int i = 0; i < 30; i++){
		neoPixels[i] = initValues[i]; // Mirrors functionality of assembly
	}
	
	// sendPixels(neoPixels);
	
}

void clearPixels(){
	for(int i = 0; i < 30; i++){
		neoPixels[i] = 0;
	}
}

void updatePixels(){
	
	for(int i = 0; i < 10; i++){
		unsigned char R = neoPixels[3*i]; //Red
		unsigned char G = neoPixels[3*i + 1]; //Green
		unsigned char B = neoPixels[3*i + 2]; //Blue
		
		sendPixels(R, G, B);
	}
}

void sendPixels(unsigned char R, unsigned char G, unsigned char B){
	byteLoop(R);
	byteLoop(G);
	byteLoop(B);
}

void byteLoop(unsigned char colorVal){
	//bin = //convert colorVal to binary 
	
	for(int i = 0; i < 8; i++){
		PORTB |= 0b00000001; 
		
		if((colorVal<<i) == 1)
		{
			sendOne();	
		}
		else
		{
			sendZero();
		}

	}
}


void sendOne()
{
	PORTB |= 0b00000001;
	
	// Put 14 clock cycle high timer here
	TCNT0 = -14;
	TCCR0A = 0x01;
	
	while((TIFR0 & (1 << TOV0)) == 0);
	
	TCCR0A = 0;
	TIFR0 = 0x1; // Clear the timer

}

void sendZero()
{
	PORTB &= 0b00000000;	
	
	// Put 7 clock cycle low timer here
	TCNT0 = -7;
	TCCR0A = 0x01;
	
	while((TIFR0 & (1 << TOV0)) == 0);
	
	TCCR0A = 0;
	TIFR0 = 0x1;
}

/*void delay(){
	
}*/
