#ifndef __LCD12864_H
#define __LCD12864_H

#include "stm32f4xx_hal.h"

extern uint8_t LCD_addr[4][8];
extern uint8_t LCD12864_p[2];

void Lcd_WriteCmd(uint8_t Cmd);								//LCD写指令
void Lcd_WriteData(uint8_t Dat);							//LCD写数据
void Lcd_Init(void);										//LCD初始化
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t*str);	//LCD显示文字
void LCD_Clear(void);										//LCD清屏
void LCD_Display_Picture(uint8_t *img);						//LCD显示图片
void LCD_Display_8Number(uint8_t numb);						//LCD显示数字
void LCD_Display_32Number(uint32_t numb);
#endif
