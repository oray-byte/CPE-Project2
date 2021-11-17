#include <avr/io.h>
#include <avr/interrupt.h>

					//R0    G0    B0,   R1    G1    B1,   R2    G2    B2,   R3    G3    B3,   R4    G4    B4,   R5    G5    B5,   R6    G6    B6,   R7    G7    B7,   R8    G8    B8,   R9    G9    B9
int initValues[] = {0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10};
int neoPixels[30];


void clearPixels();

void updatePixels();

void sendPixels();

void initNeo();

void delay();

int main(){
	DDRB = 0x00;
	PORTB = 0;
	
	
	
	return 0;
}

void initNeo(){
	for(int i = 0; i < 30; i++){
		neoPixels[i] = 0;
	}
}

void clearPixels(){
	
}

void updatePixels(){
	
}

void sendPixels(){

}



void delay(){
	int inCount = 256;
	int outCount = 256;

}