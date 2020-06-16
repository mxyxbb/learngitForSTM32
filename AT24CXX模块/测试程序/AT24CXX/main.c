#include <reg51.h>
#include <stdio.h>
#include <i2c.h>

#define uchar unsigned char
#define WRITE 0xA0                       /* 定义24C04的器件地址SLA和方向位W */
#define READ 0xA1                        /* 定义24C04的器件地址SLA和方向位R */
#define BLOCK_SIZE 32                    /* 定义指定字节个数 */
#define FALSE 0
#define TRUE ~FALSE

xdata uchar EAROMImage[BLOCK_SIZE];/* 在外部RAM中定义存储映象单元 */

/****************************************************************************
*    函数原型: bit E_address(uchar Address);
*    功    能: 向24C04写入器件地址和一个指定的字节地址。
*****************************************************************************/
bit E_address( uchar Address ) {
	I_start() ;
	if ( I_send( WRITE ) )
		return ( I_send( Address ) ) ;
	else
		return ( FALSE ) ;
}

/****************************************************************************
*    函数原型: bit E_read_block(void);
*    功    能: 从24C04中读取BLOCK_SIZE个字节的数据并转存于外部RAM存储映象
*              单元, 采用序列读操作方式从片内0地址开始连续读取数据。如果
*              24C04不接受指定的地址则返回0(FALSE)。
*****************************************************************************/
bit E_read_block( void ) {
	uchar i ;
	/* 从地址0开始读取数据 */
	if ( E_address( 0 ) ) {
		/* 发送重复启动信号 */
		I_start() ;
		if ( I_send( READ ) ) {
			for ( i=0 ; i<=BLOCK_SIZE ; i++ ) {

				EAROMImage[i] = ( I_receive() ) ;
				if ( i != BLOCK_SIZE ) I_Ack() ;
				else {
					I_clock() ;
					I_stop() ;
				}
			}
			return ( TRUE ) ;
		}
		else {
			I_stop() ;
			return ( FALSE ) ;
		}
	}
	else
		I_stop() ;
		return ( FALSE ) ;
}

/****************************************************************************
*    函数原型: void wait_5ms(void);
*    功    能: 提供5ms延时(时钟频率为12MHz)。
*****************************************************************************/
void wait_5ms( void ) {
	int i ;
	for ( i=0 ; i<1000 ; i++ ) {
		;
	}
}

/****************************************************************************
*    函数原型: bit E_write_block(void);
*    功    能: 将外部RAM存储映象单元中的数据写入到24C04的头BLOCK_SIZE个字节。
*              采用字节写操作方式, 每次写入时都需要指定片内地址。如果24C04
*              不接受指定的地址或某个传送的字节未收到应答信号ACK, 则返回0
*              (FALSE)。
*****************************************************************************/
bit E_write_block( void ) {
	uchar i ;
	for ( i=0 ; i<=BLOCK_SIZE ; i++ ) {
		if ( E_address(i) && I_send( EAROMImage[i] ) ) {
			I_stop() ;
			wait_5ms();
		}
		else
			return ( FALSE ) ;
	}
	return ( TRUE ) ;
 }

void  main() {
    SCON = 0x5a;        
    TMOD = 0x20;  
    TCON = 0x69;  
    TH1 =  0xfd; 
	I_init();                         /* I2C 总线初始化 */
	if (E_write_block())
		 printf("write I2C good.\r\n");
	else
		 printf("write I2C bad.\r\n");
	if (E_read_block()) 
		printf("read I2C good.\r\n");
	else
		 printf("read I2C bad.\r\n");
	while(1);
}
		
