;
; NeoPixel_v1.asm
;
; Created: 8/10/2020 11:03:18 AM
; Author : youngerr
;



; Replace with your application code
start:
    ; initialize stack pointer
    ldi r16,HIGH(RAMEND)
	out SPH,r16
	ldi r16,LOW(RAMEND)
	out SPL,r16

	RCALL NeoPixel_Init; General initialization tasks
	RCALL AD_Init
	LDI R24, 25
	RCALL delay
	nop
while:
    RCALL Clear_Pixels
    RCALL Read_Light
	;LDI R24,100
	MOV R17,R24; Loop counter of 25
	ANDI R17,0x3F
	LDI R16,39
	MUL R17,R16
	LDI R16,9
	CLC
	SBC R16,R1
	LDI R17,3
	MUL R17,R16
	MOV R16,R0
	LDI R26, low(NeoPixel_Array)
	LDI R27, high(NeoPixel_Array)
	LDI R24, 0x10
For_Loop:
	ST X+,R24
	DEC R16
	BRNE For_Loop 
	; Display Temperature as color of NeoPixel #9 
	LDI R26, low(NeoPixel_Array+27)
	LDI R27, high(NeoPixel_Array+27)
	RCALL Read_Temp
	LSR R24             ; Value is 0 to 127
	MOV R17,R24
	; Debug force value
	;LDI R17,90
	LDI R16,32
	CP R17,R16
	BRMI all_blue
	LDI R16,96
	CP R17,R16
	BRPL all_red
	LDI R16,64
	CP R17,R16          ; (value*2)-64
	BRMI LT_64
	LDI R16,64
	CLC 
	SBC R17,R16         ; R17 = (Rx)-64
	LSL R17             ; R17 = (Rx*2)-128
	ST X+,R17           ; Red = (value*2)-128
	MOV R17,R24
	NEG R17             ; R17 = (-Gx)
	LDI R16,96
	ADD R17,R16         ; R17 = -Gx + 96
	LSL R17             ; R17 = -2*Gx + 192
	ST X+,R17           ; Green = (-2*value) + 192
	LDI R16,0
	ST X+,R16           ; Blue = 0
	rjmp exit_if
LT_64:
    LDI R16,0
	ST X+,R16           ; Red = 0
	MOV R17,R24
	LDI R16,32
	CLC 
	SBC R17,R16         ; R17 = (Gx)-32
	LSL R17             ; R17 = (Gx*2)-64
	ST X+,R17           ; Green = (value*2)-64
	MOV R17,R24
	NEG R17             ; R17 = (-Bx)
	LDI R16,64
	ADD R17,R16         ; R17 = -Bx + 64
	LSL R17             ; R17 = -2*Bx + 128
	ST X+,R17           ; Blue = (-2*value) + 128
	rjmp exit_if
all_blue:
    LDI R16,0
	ST X+,R16           ; Red = 0
	ST X+,R16           ; Green = 0
	LDI R16,64
	ST X+,R16           ; Blue = 64
	rjmp exit_if
all_red:
    LDI R16,64
	ST X+,R16           ; Red = 64
	LDI R16,0
	ST X+,R16           ; Green = 0
	ST X+,R16           ; Blue = 0
exit_if:


	RCALL Update_Pixels

	LDI R24, 25
	RCALL delay

stop:

	rjmp while

    rjmp start
	
;**** This code is not being used ****	
    ; NeoPixel 1 RED
	LDI R24, 0x3f
	LDI R22, 0
	LDI R20, 0
	RCALL Send_Pixel
;	LDI R24, 25
;	RCALL delay
	; NeoPixel 1 Green
	LDI R24, 0
	LDI R22, 0x3f
	LDI R20, 0
	RCALL Send_Pixel
;	LDI R24, 25
;	RCALL delay

	; NeoPixel 1 Blue
	LDI R24, 0
	LDI R22, 0
	LDI R20, 0x3f
	RCALL Send_Pixel
	LDI R24, 50
	RCALL delay

	rjmp while

    rjmp start
	              ; R0    G0    B0,   R1    G1    B1,   R2    G2    B2,   R3    G3    B3,   R4    G4    B4,   R5    G5    B5,   R6    G6    B6,   R7    G7    B7,   R8    G8    B8,   R9    G9    B9
NP_Init_values: .DB 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x10, 0x00, 0x20, 0x20, 0x00, 0x10

; Function to go through and initialize each pixel 
NeoPixel_Init:
	;These values are to set the initial light values
    PUSH R31
	PUSH R30
	PUSH R27
	PUSH R26
	PUSH R24 
	PUSH R16
	; set PB0 to an output, initial value is '0'
	CBI 5,0
	SBI 4,0
	LDI R26, low(NeoPixel_Array); Corresponds to XL
	LDI R27, high(NeoPixel_Array); Corresponds to XH
	LDI R30, low(NP_Init_values<<1); Corresponds to ZL
	LDI R31, high(NP_Init_values<<1); Corresponds to ZH
	LDI R24, 30
; Loops through and sets each pixel
NP_For_Loop:
	LPM R16,Z+; Get location of Z
	ST X+,R16; Transfer value of Z
	DEC R24
	BRNE NP_For_Loop; If all rgb values for pixels are set, branch
	RCALL Update_Pixels
	; Releases memory to work on next pixel
	POP R16
	POP R24
	POP R26
	POP R27
	POP R30
	POP R31
	RET

Clear_Pixels:
    PUSH R27
	PUSH R26
    PUSH R24
	PUSH R16
	LDI R26, low(NeoPixel_Array)
	LDI R27, high(NeoPixel_Array)
	LDI R24, 30
	LDI R16,0
CP_For_Loop:
	ST X+,R16
	DEC R24
	BRNE CP_For_Loop 
	POP R16
	POP R24
	POP R26
	POP R27
	RET	

Update_Pixels:
	;Push local variable storage
    PUSH R27; Low of array (current position)
	PUSH R26; High of array (current position)
    PUSH R24; R
	PUSH R22; G
	PUSH R20; B
	PUSH R16; Counter
	LDI R16,10
	LDI R26, low(NeoPixel_Array)
	LDI R27, high(NeoPixel_Array)
;Loops through 10 times for each pixel
UP_For_Loop:
	; Loading up next rgb values
	LD R24,X+
	LD R22,X+
	LD R20,X+
	RCALL Send_Pixel
	DEC R16
	BRNE UP_For_Loop
	;Releases local variable memory
	POP R16
	POP R20
	POP R22
	POP R24
	POP R26
	POP R27
	RET


Send_Pixel:
	; Three parameters: R = R24, G = R22 and B = R20
	; G needs to be sent first, then R and the B
	PUSH R24; R
	PUSH R23
	PUSH R22; G
	PUSH R21
	PUSH R20; B
	RCALL byte_loop
	MOV R22, R24
	RCALL byte_loop
	MOV R22, R20
	RCALL byte_loop
	POP R20
	POP R21
	POP R22
	POP R23
	POP R24
	RET

; Loop to change pixels byte at a time
byte_loop:
	LDI R23,8
	;set PB0 to one
next_bit:
	SBI 5,0
	ROL R22
	BRCS send_one; Branch if c = 1
; send_zero:
;	LDI R21,2           ; 7 clock cycles high
;wait_zero_high:
;	DEC R21
;	BRNE wait_zero_high
	; clear PB0 to zero
	CBI 5,0
	LDI R21,1          ; 14 clock cycles low
wait_zero_low:
	DEC R21
	BRNE wait_zero_low
	RJMP exit_zero
send_one:
	LDI R21,2           ; 14 clock cycles high
wait_one_high:
	DEC R21
	BRNE wait_one_high
	; clear PB0 to zero
	CBI 5,0
;	LDI R21,2          ; 7 clock cycles low
;wait_one_low:
;	DEC R21
;	BRNE wait_one_low
exit_zero:
	DEC R23
	BRNE next_bit
	RET	

; For light and temperature sensor initializations
AD_Init:
    PUSH R16
	LDI R16,0x96     ; Enable='1', Start ='0', Auto-Trigger='0', Flag='1' (to clear flag), IE='0' (disabled), Prescale=clock/64
	STS ADCSRA,R16
	LDI R16,0xE0     ; Ref = Internal, Left Justifed data, MUX=channel A0
	STS ADMUX,R16
	LDI R16,0x81
	STS DIDR0,R16
	STS PORTF,R16
	POP R16
	RET

Read_Light:
    PUSH R16
	LDS R16,ADMUX
	ANDI R16,0xE0
	STS ADMUX,R16
	LDS R16,ADCSRA
	ORI R16,0x50
	STS ADCSRA,R16
RL_while:
	LDS R16,ADCSRA
	ANDI R16,(1<<4)
	BREQ RL_while
	LDS R24,ADCH
	POP R16
	RET

Read_Temp:
    PUSH R16
	LDS R16,ADMUX
	ANDI R16,0xE0
	ORI R16,0x07
	STS ADMUX,R16
	LDS R16,ADCSRA
	ORI R16,0x50
	STS ADCSRA,R16
RT_while_1:
	LDS R16,ADCSRA
	ANDI R16,(1<<4)
	BREQ RT_while_1
	LDS R24,ADCH
	POP R16
	RET

; Delay for about 1.6 million machine cycles
delay:
   PUSH R24; Initial value of 25
   PUSH R18
   PUSH R17
d1:
    LDI R17,0xFF; Value of 255
d2:
	LDI R18,0xFF; Value of 255
d3:
	DEC R18
	BRNE d3
	DEC R17
	BRNE d2
	DEC R24
	BRNE d1
	POP R17
	POP R18
	POP R24
    ret

.dseg
NeoPixel_Array: .byte 30   ; reserves 30 bytes for neopixels (RGB to 10 pixels)