//main.c
#include <SI_EFM8BB1_Register_Enums.h>
void InitDevice(void)
{
	
//Disable Watchdog Timer with key sequence
WDTCN = 0xDE; //First key
WDTCN = 0xAD; //Second key

//Setup Defaults for P1
P1MDOUT = P1MDOUT_B0__OPEN_DRAIN | P1MDOUT_B1__OPEN_DRAIN
| P1MDOUT_B2__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN
| P1MDOUT_B4__PUSH_PULL | P1MDOUT_B5__OPEN_DRAIN
| P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B7__OPEN_DRAIN;
//Enable Weak Pullups
XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;
//Disable Interrupts
IE = IE_EA__DISABLED | IE_EX0__DISABLED | IE_EX1__DISABLED
| IE_ESPI0__DISABLED | IE_ET0__DISABLED | IE_ET1__DISABLED
| IE_ET2__ENABLED | IE_ES0__DISABLED;
}
unsigned int timer = 180;
unsigned char c = 0;
void LCD_CMD_write(unsigned x){// lets us setup the display 
	unsigned char a;
	P0 &= ~(1<<5);// clear enable
	for(a = 0; a<100;a++);
	P0 &= ~(1<<3);//RS is in command mode not dat_a P0.3
	P0 &= ~(1<<4);// RW we are writing to the LCD P0.4
	for(a = 0; a<100;a++);
	P1 = x;// put the dat_a on wires
	P0 |= (1<<5);// set enable to 1 P0.5
	for(a = 0; a<100;a++);// for loop 50 iterations
	P0 &= ~(1<<5);// clear enable
	for(a = 0; a<100;a++);//delay again
	
}

void LCD_Data_Write(unsigned x){//lets us write a letter to the display
	unsigned char a;
	P0 &= ~(1<<5);// set enable to zero
	for(a = 0; a<100;a++);// delay
	P0 |= (1<<3);// Rs in dat_a mode set to 1
	P0 &= ~(1<<4); // RW is in write mode
	for(a = 0; a<100;a++);// delay before we load dat_a
	P1 = x;//load the Data
	P0 |= (1<<5);//set enable high
	for(a = 0; a<100;a++);// for loop 50 iterations
	P0 &= ~(1<<5);//set enable zero
	for(a = 0; a<100;a++);// for loop 50 iterations
	
}
void ISR_OVERFLOW (void) interrupt 1{
	unsigned int x;
	TCON &= ~(1<<4);
	TH0 = 0xEC;
	TL0 = 0x0F;
	TCON |= (1<<4);
	P2 |= (1<<1);//set P2.1 high
	for(x = 0; x<timer; x++);
	P2 &= ~(1<<1); //clear P2.1 low
}
void ISR_OVERFLOW1 (void) interrupt 3{
	TCON &= ~(1<<6);//stop timer 1 clears
	TH1 = 0x38;//reload bits in 16bit mode
	TL1 = 0x98;
	TCON |= (1 << 6);//start timer again sets
	c++; //increments the delay global variable c
}
void DelayMs(unsigned o){
	c = 0;
	while(c < o);
}
void main(void){
	unsigned char z,d;
	InitDevice();
	IE = 0x8A;//enables overflow inter 0 & 1
	TMOD = 0x11;//enables both timer 1 and 0
	TH0 = 0xEC;//timer runs 20ms
	TL0 = 0x0F;//delays 20ms
	TH1 = 0x38;//delay 200ms
	TL1 = 0x9E;
	TCON |= (1<<6);//runs timer 1
	TCON |= (1<<4);//runs timer 0
	LCD_CMD_write(0x38);//inizaltion LCD to be set up
	LCD_CMD_write(0x06);
	LCD_CMD_write(0x0E);
	LCD_CMD_write(0x01);
	LCD_CMD_write(0x0C);
	for(z = 0;z<200;z++);
	//LCD_Data_Write(239);//gasping emoji
	//LCD_Data_Write(239);//gasping emoji
	//LCD_CMD_write(0xC0);//sends to other line
	while(1){
		LCD_CMD_write(0x0C);
		if((!(P0 & (1<<2))) && (timer > 180)){
			P0 |= (1<<1);
			timer -= 180;
			for(d = 0; d<150; d++);
			LCD_Data_Write('P');
			LCD_Data_Write('e');
			LCD_Data_Write('r');
			LCD_Data_Write('s');
			LCD_Data_Write('o');
			LCD_Data_Write('n');
			LCD_CMD_write(0xC0);
			LCD_Data_Write('D');
			LCD_Data_Write('e');
			LCD_Data_Write('t');
			LCD_Data_Write('e');
			LCD_Data_Write('c');
			LCD_Data_Write('t');
			LCD_Data_Write('e');
			LCD_Data_Write('d');
			while(!(P0 & (1 << 2)));
			LCD_CMD_write(0x01);
			LCD_CMD_write(0x0C);
		}
		if((P0 & (1<<2)) && (timer < 360)){
			P0 &= ~(1<<1);
			for(d = 0; d<150; d++);
			LCD_Data_Write('P');
			LCD_Data_Write('e');
			LCD_Data_Write('r');
			LCD_Data_Write('s');
			LCD_Data_Write('o');
			LCD_Data_Write('n');
			LCD_CMD_write(0xC0);
			LCD_Data_Write('N');
			LCD_Data_Write('o');
			LCD_Data_Write('t');
			LCD_Data_Write(' ');
			LCD_Data_Write('D');
			LCD_Data_Write('e');
			LCD_Data_Write('t');
			LCD_Data_Write('e');
			LCD_Data_Write('c');
			LCD_Data_Write('t');
			LCD_Data_Write('e');
			LCD_Data_Write('d');
			DelayMs(25); //5second
			timer += 180;
			while(P0 & (1 << 2));
			LCD_CMD_write(0x01);
			LCD_CMD_write(0x0C);
		}
	}
}
