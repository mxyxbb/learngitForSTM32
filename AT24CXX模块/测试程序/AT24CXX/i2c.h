#define uchar unsigned char
#define uint  unsigned int

void delay( void ); 
void I_stop( void );
void I_init( void );
void I_start( void );
bit I_clock( void );
void I_Ack( void );
bit I_send( uchar I_data );
uchar I_receive( void );

