/**
 *
 * This file is a modified version of the HW_Teensy3.h created by Paul Stoffregen.
 *
 * Teensy 3.x pin definitions created by Dawnmist
 * http://forum.pjrc.com/threads/18002-Teensy-3-0-driving-an-SSD1289-with-utft?p=34719&viewfull=1#post34719
 *
 * This file only supports the B and D ports as defined by Dawnmist for 8-bit and 16-bit display modules.
 * Serial display modules are also supported.
 * 
 * NOTE: This file has only been tested on a Teensy 3.1
 *
**/
#define digitalPinToBitMask(P)     (BIT(PIN_MAP[P].gpio_bit))
#define digitalPinToPort(P)        (PIN_MAP[P].gpio_device)
#define portOutputRegister(P)      (&(P->regs->ODR))

#define digitalWriteFast(pin,val)

#define setBit(val) PIN_MAP[__p1].gpio_device->regs->BSRR=((B_SDA)<<((val==0)?16:0))

#define HighCLK PIN_MAP[__p2].gpio_device->regs->BSRR=B_SCL
#define LowCLK PIN_MAP[__p2].gpio_device->regs->BRR=B_SCL
#define CLK_pulse_low LowCLK; HighCLK;
typedef unsigned int word;

#define DB_0 0
#define DB_1 1
#define DB_2 2
#define DB_3 3
#define DB_4 8
#define DB_5 9
#define DB_6 10
#define DB_7 11
#define DB_8 15
#define DB_9 16
#define DB_10 17
#define DB_11 18
#define DB_12 19
#define DB_13 20
#define DB_14 21
#define DB_15 22


// *** Hardware specific functions ***
void UTFT::_hw_special_init()
{
//	__p1 = RS;
//	__p2 = WR;
//	__p3 = CS;
//	__p4 = RST;
//	__p5 = SER;
    pinMode(__p4,OUTPUT); // manual reset for safetly
    digitalWrite(__p4,HIGH);
    digitalWrite(__p4,LOW);
    digitalWrite(__p4,HIGH);
	if (display_transfer_mode == SERIAL_4PIN)
	{
		display_transfer_mode=1;
		display_serial_mode=SERIAL_4PIN;
	}
	if (display_transfer_mode == SERIAL_5PIN)
	{
		display_transfer_mode=1;
		display_serial_mode=SERIAL_5PIN;
	}

	if (display_transfer_mode!=1)
	{
        
		_set_direction_registers(display_transfer_mode);
		P_RS	= portOutputRegister(digitalPinToPort(__p1)); // P_x = outputregister des pins
		P_WR	= portOutputRegister(digitalPinToPort(__p2));
		P_CS	= portOutputRegister(digitalPinToPort(__p3));
		P_RST	= portOutputRegister(digitalPinToPort(__p4));
	if (display_transfer_mode==LATCHED_16)
		{
			P_ALE	= portOutputRegister(digitalPinToPort(__p5));
    	cbi(P_ALE, B_ALE);
		//	pinMode(8,OUTPUT);
		//	gpio_write_bit(8, LOW);
		}
	}
	else
	{
        
		P_SDA	= portOutputRegister(digitalPinToPort(__p1));
		P_SCL	= portOutputRegister(digitalPinToPort(__p2));
		P_CS	= portOutputRegister(digitalPinToPort(__p3));
       
		if (__p4 = NOTINUSE)
		  {P_RST = P_SCL; B_RST=1U<<16;} //Like NULL
          
		else
		  P_RST	= portOutputRegister(digitalPinToPort(__p4));
		
		if (display_serial_mode!=SERIAL_4PIN)
		  P_RS	= portOutputRegister(digitalPinToPort(__p5));
		 
	}
// TEST
    
}

void UTFT::LCD_Writ_Bus(char VH,char VL, byte mode)
{
	switch (mode)
	{
	case 1:
		if (display_serial_mode==SERIAL_4PIN)
		{
			if (VH==1)
				sbi(P_SDA, B_SDA);
			else
				cbi(P_SDA, B_SDA);
			pulse_low(P_SCL, B_SCL);
		}
		else
		{
			if (VH==1)
				sbi(P_RS, B_RS);
			else
				cbi(P_RS, B_RS);
		}

		if (VL & 0x80)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		if (VL & 0x40)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		if (VL & 0x20)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		if (VL & 0x10)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		if (VL & 0x08)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		if (VL & 0x04)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		if (VL & 0x02)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		if (VL & 0x01)
			sbi(P_SDA, B_SDA);
		else
			cbi(P_SDA, B_SDA);
		pulse_low(P_SCL, B_SCL);
		break;
	case 8:
		GPIOB->regs->ODR = ((GPIOB->regs->ODR & 0x00FF) | ((VH << 8) & 0xFF00)); //Hopefully put VH on PB[7:0]
//		(&GPIO_PDOR) = VH;
		pulse_low(P_WR, B_WR);
		GPIOB->regs->ODR = ((GPIOB->regs->ODR & 0x00FF) | ((VL << 8) & 0xFF00)); //Hopefully put VL on PB[7:0]
		//nt8_t *)(&GPIOD_PDOR) = VL;
		pulse_low(P_WR, B_WR);
		
		break;
	case 16:
		((VH & 0x80) != 0) ? gpio_write_bit(GPIOA, 13, HIGH) : gpio_write_bit(GPIOA, 13, LOW);
		((VH & 0x40) != 0) ? gpio_write_bit(GPIOA, 14, HIGH) : gpio_write_bit(GPIOA, 14, LOW);
		((VH & 0x20) != 0) ? gpio_write_bit(GPIOA, 15, HIGH) : gpio_write_bit(GPIOA, 15, LOW);
		((VH & 0x10) != 0) ? gpio_write_bit(GPIOB, 3, HIGH) : gpio_write_bit(GPIOB, 3, LOW);
		((VH & 0x08) != 0) ? gpio_write_bit(GPIOB, 4, HIGH) : gpio_write_bit(GPIOB, 4, LOW);
		((VH & 0x04) != 0) ? gpio_write_bit(GPIOB, 5, HIGH) : gpio_write_bit(GPIOB, 5, LOW);
		((VH & 0x02) != 0) ? gpio_write_bit(GPIOB, 6, HIGH) : gpio_write_bit(GPIOB, 6, LOW);
		((VH & 0x01) != 0) ? gpio_write_bit(GPIOB, 7, HIGH) : gpio_write_bit(GPIOB, 7, LOW);
		((VL & 0x80) != 0) ? gpio_write_bit(GPIOA, 0, HIGH) : gpio_write_bit(GPIOA, 0, LOW);
		((VL & 0x40) != 0) ? gpio_write_bit(GPIOA, 1, HIGH) : gpio_write_bit(GPIOA, 1, LOW);
		((VL & 0x20) != 0) ? gpio_write_bit(GPIOA, 2, HIGH) : gpio_write_bit(GPIOA, 2, LOW);
		((VL & 0x10) != 0) ? gpio_write_bit(GPIOA, 3, HIGH) : gpio_write_bit(GPIOA, 3, LOW);
		((VL & 0x08) != 0) ? gpio_write_bit(GPIOB, 0, HIGH) : gpio_write_bit(GPIOB, 0, LOW);
		((VL & 0x04) != 0) ? gpio_write_bit(GPIOB, 2, HIGH) : gpio_write_bit(GPIOB, 2, LOW);
		((VL & 0x02) != 0) ? gpio_write_bit(GPIOB, 10, HIGH) : gpio_write_bit(GPIOB, 10, LOW);
		((VL & 0x01) != 0) ? gpio_write_bit(GPIOB, 11, HIGH) : gpio_write_bit(GPIOB, 11, LOW);

		pulse_low(P_WR, B_WR);
		break;	

	}
}

void UTFT::_set_direction_registers(byte mode)
{

    pinMode(DB_8, OUTPUT);
    pinMode(DB_9, OUTPUT);
    pinMode(DB_10, OUTPUT);
    pinMode(DB_11, OUTPUT);
    pinMode(DB_12, OUTPUT);
    pinMode(DB_13, OUTPUT);
    pinMode(DB_14, OUTPUT);
    pinMode(DB_15, OUTPUT);
    if (mode == 16)
    {
	pinMode(DB_0, OUTPUT);
	pinMode(DB_1, OUTPUT);
	pinMode(DB_2, OUTPUT);
	pinMode(DB_3, OUTPUT);
	pinMode(DB_4, OUTPUT);
	pinMode(DB_5, OUTPUT);
	pinMode(DB_6, OUTPUT);
	pinMode(DB_7, OUTPUT);
    }

}
void UTFT::_fast_fill_16(int ch, int cl, long pix)
{
	long blocks;

		((ch & 0x80) != 0) ? gpio_write_bit(GPIOA, 13, HIGH) : gpio_write_bit(GPIOA, 13, LOW);
                ((ch & 0x40) != 0) ? gpio_write_bit(GPIOA, 14, HIGH) : gpio_write_bit(GPIOA, 14, LOW);
                ((ch & 0x20) != 0) ? gpio_write_bit(GPIOA, 15, HIGH) : gpio_write_bit(GPIOA, 15, LOW);
                ((ch & 0x10) != 0) ? gpio_write_bit(GPIOB, 3, HIGH) : gpio_write_bit(GPIOB, 3, LOW);
                ((ch & 0x08) != 0) ? gpio_write_bit(GPIOB, 4, HIGH) : gpio_write_bit(GPIOB, 4, LOW);
                ((ch & 0x04) != 0) ? gpio_write_bit(GPIOB, 5, HIGH) : gpio_write_bit(GPIOB, 5, LOW);
                ((ch & 0x02) != 0) ? gpio_write_bit(GPIOB, 6, HIGH) : gpio_write_bit(GPIOB, 6, LOW);
                ((ch & 0x01) != 0) ? gpio_write_bit(GPIOB, 7, HIGH) : gpio_write_bit(GPIOB, 7, LOW);
                ((cl & 0x80) != 0) ? gpio_write_bit(GPIOA, 0, HIGH) : gpio_write_bit(GPIOA, 0, LOW);
                ((cl & 0x40) != 0) ? gpio_write_bit(GPIOA, 1, HIGH) : gpio_write_bit(GPIOA, 1, LOW);
                ((cl & 0x20) != 0) ? gpio_write_bit(GPIOA, 2, HIGH) : gpio_write_bit(GPIOA, 2, LOW);
                ((cl & 0x10) != 0) ? gpio_write_bit(GPIOA, 3, HIGH) : gpio_write_bit(GPIOA, 3, LOW);
                ((cl & 0x08) != 0) ? gpio_write_bit(GPIOB, 0, HIGH) : gpio_write_bit(GPIOB, 0, LOW);
                ((cl & 0x04) != 0) ? gpio_write_bit(GPIOB, 2, HIGH) : gpio_write_bit(GPIOB, 2, LOW);
                ((cl & 0x02) != 0) ? gpio_write_bit(GPIOB, 10, HIGH) : gpio_write_bit(GPIOB, 10, LOW);
                ((cl & 0x01) != 0) ? gpio_write_bit(GPIOB, 11, HIGH) : gpio_write_bit(GPIOB, 11, LOW);


	blocks = pix/16;
	for (int i=0; i<blocks; i++)
	{
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);
		if(!(i%1000)) 
		{
      yield();
    }		
	}
	if ((pix % 16) != 0)
		for (int i=0; i<(pix % 16); i++)
		{
			pulse_low(P_WR, B_WR);
		}
}

void UTFT::_fast_fill_8(int ch, long pix)
{
	long blocks;
	
	((ch & 0x80) != 0) ? gpio_write_bit(GPIOA, 13, HIGH) : gpio_write_bit(GPIOA, 13, LOW);
        ((ch & 0x40) != 0) ? gpio_write_bit(GPIOA, 14, HIGH) : gpio_write_bit(GPIOA, 14, LOW);
        ((ch & 0x20) != 0) ? gpio_write_bit(GPIOA, 15, HIGH) : gpio_write_bit(GPIOA, 15, LOW);
        ((ch & 0x10) != 0) ? gpio_write_bit(GPIOB, 3, HIGH) : gpio_write_bit(GPIOB, 3, LOW);
        ((ch & 0x08) != 0) ? gpio_write_bit(GPIOB, 4, HIGH) : gpio_write_bit(GPIOB, 4, LOW);
        ((ch & 0x04) != 0) ? gpio_write_bit(GPIOB, 5, HIGH) : gpio_write_bit(GPIOB, 5, LOW);
        ((ch & 0x02) != 0) ? gpio_write_bit(GPIOB, 6, HIGH) : gpio_write_bit(GPIOB, 6, LOW);
        ((ch & 0x01) != 0) ? gpio_write_bit(GPIOB, 7, HIGH) : gpio_write_bit(GPIOB, 7, LOW);

	
	blocks = pix/16;
	for (int i=0; i<blocks; i++)
	{
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		if(!(i%1000)) 
		{
      yield();
    }
	}
	if ((pix % 16) != 0)
		for (int i=0; i<(pix % 16); i++)
		{
			pulse_low(P_WR, B_WR);pulse_low(P_WR, B_WR);
		}
}
