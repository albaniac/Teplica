case S6D0164:
	LCD_Write_COM_DATA(0x11,0x001A);
	LCD_Write_COM_DATA(0x12,0x3121);
	LCD_Write_COM_DATA(0x13,0x006C);
	LCD_Write_COM_DATA(0x14,0x4249);

	LCD_Write_COM_DATA(0x10,0x0800);
	delay(10);
	LCD_Write_COM_DATA(0x11,0x011A);
	delay(10);	
	LCD_Write_COM_DATA(0x11,0x031A);
	delay(10);
	LCD_Write_COM_DATA(0x11,0x071A);
	delay(10);
	LCD_Write_COM_DATA(0x11,0x0F1A);
	delay(10);	
	LCD_Write_COM_DATA(0x11,0x0F3A);
	delay(30);

	LCD_Write_COM_DATA(0x01,0x011C);
	LCD_Write_COM_DATA(0x02,0x0100);
	LCD_Write_COM_DATA(0x03,0x1030);
	LCD_Write_COM_DATA(0x07,0x0000);
	LCD_Write_COM_DATA(0x08,0x0808);
	LCD_Write_COM_DATA(0x0B,0x1100);
	LCD_Write_COM_DATA(0x0C,0x0000);

	LCD_Write_COM_DATA(0x0F,0x1401);
	LCD_Write_COM_DATA(0x15,0x0000);
	LCD_Write_COM_DATA(0x20,0x0000);
	LCD_Write_COM_DATA(0x21,0x0000);
	

	LCD_Write_COM_DATA(0x36,0x00AF); 
	LCD_Write_COM_DATA(0x37,0x0000); 	
	LCD_Write_COM_DATA(0x38,0x00DB);
	LCD_Write_COM_DATA(0x39,0x0000); 
	
	LCD_Write_COM_DATA(0x0F,0x0B01);
	LCD_Write_COM_DATA(0x07,0x0016);	
	LCD_Write_COM_DATA(0x07,0x0017);

	LCD_Write_COM(0x22);
	break;
