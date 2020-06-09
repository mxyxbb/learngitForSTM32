#include "LCD12864.h"
#include "spi.h"
//------------------------------------------------------------------


uint8_t LCD_addr[4][8]={
	{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},  	//第一行
	{0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97},		//第二行
	{0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},		//第三行
	{0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F}		//第四行
	};
	
uint8_t LCD12864_p[2];	

//---------------------------------------------------------------



/*! 
 *  @brief      LCD写指令
 *  @since      v1.0
 *  @param  Cmd   要写入的指令
 *  @author     冰糖
 改编自CSDN: https://blog.csdn.net/as480133937/article/details/97765912
 */
void Lcd_WriteCmd(uint8_t Cmd )
{
	LCD12864_p[0] = 0xf8;//write command
    HAL_Delay(1);    //由于我们没有写LCD正忙的检测，所以直接延时1ms，使每次写入数据或指令间隔大于1ms 便可不用写忙状态检测
    HAL_SPI_Transmit(&hspi1,LCD12864_p,1,10);            //11111,RW(0),RS(0),0  
	LCD12864_p[0] = 0xf0&Cmd;
    HAL_SPI_Transmit(&hspi1,LCD12864_p,1,10);      //高四位
	LCD12864_p[0] = Cmd<<4;
    HAL_SPI_Transmit(&hspi1,LCD12864_p,1,10);   //低四位(先执行<<)
}




/*! 
 *  @brief      LCD写数据
 *  @since      v1.0
 *  @param  Cmd   要写入的数据
 *  @author     冰糖
 */
void Lcd_WriteData(uint8_t Dat )
{
	LCD12864_p[0] = 0xFA;//write command
	HAL_Delay(1);    //由于我们没有写LCD正忙的检测，所以直接延时1ms，使每次写入数据或指令间隔大于1ms 便可不用写忙状态检测
	HAL_SPI_Transmit(&hspi1,LCD12864_p,1,10);            //11111,RW(0),RS(0),0  
	LCD12864_p[0] = 0xf0&Dat;
	HAL_SPI_Transmit(&hspi1,LCD12864_p,1,10);      //高四位
	LCD12864_p[0] = Dat<<4;
	HAL_SPI_Transmit(&hspi1,LCD12864_p,1,10);   //低四位(先执行<<)
}

/*! 
 *  @brief      LCD初始化
 *  @since      v1.0
 *  @param  
 *  @author     冰糖
 */
void Lcd_Init(void)
{ 
	HAL_Delay(50);   	//等待液晶自检（延时>40ms）
	Lcd_WriteCmd(0x30);        //功能设定:选择基本指令集  ，选择8bit数据流
  HAL_Delay(1);			//延时>137us 
  Lcd_WriteCmd(0x0c);        //开显示
  HAL_Delay(1);	    //延时>100us
  Lcd_WriteCmd(0x01);        //清除显示，并且设定地址指针为00H
  HAL_Delay(30);   	//延时>10ms
	Lcd_WriteCmd(0x06);        //每次地址自动+1，初始化完成
}





/*! 
 *  @brief      显示字符或汉字
 *  @since      v1.0
 *  @param  x: row(0~3)
 *  @param  y: line(0~7) 
 *  @param 	str: 要显示的字符或汉字
 *  @author     Z小旋
 */
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t*str)
{ 
	Lcd_WriteCmd(LCD_addr[x][y]); //写初始光标位置
	while(*str>0)
  { 
    Lcd_WriteData(*str);    //写数据
    str++;     
  }
}






/*! 
 *  @brief      清屏函数
 *  @since      v1.0
 *  @param  None
 *  @author     Z小旋
 */
void LCD_Clear(void)
{
	Lcd_WriteCmd(0x01);			//清屏指令
	HAL_Delay(2);				//延时以待液晶稳定【至少1.6ms】
}







/*! 
 *  @brief      显示图片
 *  @since      v1.0
 *  @param  *pic   图片地址
 *  @author   
这里要注意  在显示一幅图片之后，要加上2s左右延时，否则不会有图片显示
 */
void LCD_Display_Picture(uint8_t *img)
{
	uint8_t x,y,i;
	Lcd_WriteCmd(0x34);		//切换到扩充指令
	Lcd_WriteCmd(0x34);		//关闭图形显示
	for(i = 0; i < 1; i++)   //上下屏写入
	{
		for(y=0;y<32;y++)   //垂直Y写32次
		{  
			for(x=0;x<8;x++)   //横向X写8次
			{
				Lcd_WriteCmd(0x80 + y);		//行地址
				Lcd_WriteCmd(0x80 + x+i);		//列地址
				Lcd_WriteData(*img ++);		//写高位字节数据 D15－D8   
				Lcd_WriteData(*img ++);		//写低位字节数据 D7－D0
			}
		}
	}
	Lcd_WriteCmd(0x36);//打开图形显示		
	Lcd_WriteCmd(0x30);        //切换回基本指令
}


/*! 
 *  @brief      LCD显示uint8数字
 *  @since      v1.0
 *  @param  numb   要显示的数字
 *  @author     冰糖
 */
void LCD_Display_8Number(uint8_t numb)
{	
	uint8_t ch[3];
	ch[0] = numb/100 + 0x30; 	// 取百位
	ch[1] = (numb/10)%10  + 0x30;	// 取十位
	ch[2] = numb%10  + 0x30;	// 取个位
	

	Lcd_WriteData(ch[0]);
	Lcd_WriteData(ch[1]);
	Lcd_WriteData(ch[2]);
}


/*! 
 *  @brief      LCD显示uint32数字
 *  @since      v1.0
 *  @param  numb   要显示的数字
 *  @author     冰糖
 */
void LCD_Display_32Number(uint32_t numb)
{

	uint8_t ch[10];
	uint8_t i=0;
	uint8_t j=0;
	
	
	ch[0] = (numb/1000000000)%10 + 0x30; 	// 取十亿位
	ch[1] = (numb/100000000)%10 + 0x30; 	// 取亿位
	ch[2] = (numb/10000000)%10 + 0x30; 	// 取千万位
	ch[3] = (numb/1000000)%10 + 0x30; 	// 取百万位
	ch[4] = (numb/100000)%10 + 0x30; 	// 取十万位
	ch[5] = (numb/10000)%10 + 0x30; 	// 取万位
	ch[6] = (numb/1000)%10 + 0x30; 	// 取千位
	ch[7] = (numb/100)%10 + 0x30; 	// 取百位
	ch[8] = (numb/10)%10  + 0x30;	// 取十位
	ch[9] = numb%10  + 0x30;	// 取个位
	
	//去零显示
//	j = 0;
//	if(ch[j++] != '0')		//如果十亿位不是零
//	{
		for(i=0;i<=9;i++)
		Lcd_WriteData(ch[i]);	//显示包括十亿位的所有数字
//	}
//	else if (ch[j++] != '0')//如果亿位不是零
//	{
//		for(i=j-1;i<=9;i++)
//		Lcd_WriteData(ch[i]);//显示包括亿位的所有数字
//	}
//	else if (ch[j++] != '0')//千万位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
//	else if (ch[j++] != '0')//百万位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
//	else if (ch[j++] != '0')//十万位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
//	else if (ch[j++] != '0')//万位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
//	else if (ch[j++] != '0')//千位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
//	else if (ch[j++] != '0')//百位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
//	else if (ch[j++] != '0')//十位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
//	else if (ch[j++] != '0')//个位去零
//	{
//		for(i=j-1;i<=9;i++)
//			Lcd_WriteData(ch[i]);
//	}
	
	
	/*等价于上式
	if(ch[0] != '0') 
	{
		for(i=0;i<10;i++)
			Lcd_WriteData(ch[i]);
	}
	else if(ch[1] != '0') 
	{
		for(i=1;i<10;i++)
			Lcd_WriteData(ch[i]);
	}
	else  
		...
*/	
}
