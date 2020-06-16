#include <reg51.h>
#include <stdio.h>
#include <i2c.h>

#define uchar unsigned char
#define WRITE 0xA0                       /* ����24C04��������ַSLA�ͷ���λW */
#define READ 0xA1                        /* ����24C04��������ַSLA�ͷ���λR */
#define BLOCK_SIZE 32                    /* ����ָ���ֽڸ��� */
#define FALSE 0
#define TRUE ~FALSE

xdata uchar EAROMImage[BLOCK_SIZE];/* ���ⲿRAM�ж���洢ӳ��Ԫ */

/****************************************************************************
*    ����ԭ��: bit E_address(uchar Address);
*    ��    ��: ��24C04д��������ַ��һ��ָ�����ֽڵ�ַ��
*****************************************************************************/
bit E_address( uchar Address ) {
	I_start() ;
	if ( I_send( WRITE ) )
		return ( I_send( Address ) ) ;
	else
		return ( FALSE ) ;
}

/****************************************************************************
*    ����ԭ��: bit E_read_block(void);
*    ��    ��: ��24C04�ж�ȡBLOCK_SIZE���ֽڵ����ݲ�ת�����ⲿRAM�洢ӳ��
*              ��Ԫ, �������ж�������ʽ��Ƭ��0��ַ��ʼ������ȡ���ݡ����
*              24C04������ָ���ĵ�ַ�򷵻�0(FALSE)��
*****************************************************************************/
bit E_read_block( void ) {
	uchar i ;
	/* �ӵ�ַ0��ʼ��ȡ���� */
	if ( E_address( 0 ) ) {
		/* �����ظ������ź� */
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
*    ����ԭ��: void wait_5ms(void);
*    ��    ��: �ṩ5ms��ʱ(ʱ��Ƶ��Ϊ12MHz)��
*****************************************************************************/
void wait_5ms( void ) {
	int i ;
	for ( i=0 ; i<1000 ; i++ ) {
		;
	}
}

/****************************************************************************
*    ����ԭ��: bit E_write_block(void);
*    ��    ��: ���ⲿRAM�洢ӳ��Ԫ�е�����д�뵽24C04��ͷBLOCK_SIZE���ֽڡ�
*              �����ֽ�д������ʽ, ÿ��д��ʱ����Ҫָ��Ƭ�ڵ�ַ�����24C04
*              ������ָ���ĵ�ַ��ĳ�����͵��ֽ�δ�յ�Ӧ���ź�ACK, �򷵻�0
*              (FALSE)��
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
	I_init();                         /* I2C ���߳�ʼ�� */
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
		
