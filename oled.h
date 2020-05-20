#ifndef _OLED12864_H
#define _OLED12864_H

  #include"iap15w4k58s4.h"
  #include "intrins.h" 
 	#define byte  unsigned char
	#define word  unsigned int
	#define dword unsigned long  
	
 void LCD_Init(void);
 void LCD_CLS(void);
 void LCD_P6x8Str(byte x,byte y,byte ch[]);
 void LCD_P8x16Str(byte x,byte y,byte ch[]);
void LCD_P8x16Str_1(unsigned char x,unsigned char y,unsigned char ch[]);
 void LCD_P14x16Ch(byte x,byte y,byte N);  	  
 void LCD_Fill(byte dat);
#endif

