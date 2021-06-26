/*
 * LCD.c
 *
 *  Created on: Oct 10, 2019
 *      Author: Miguelito
 */

#include <LCD.h>

#define LCDCFG_NOT_GPIO 3
#define LCDCFG_PULL_DOWN 3
#define LCDCFG_PIN_DATA4 10
#define LCDCFG_PIN_DATA5 12
#define LCDCFG_PIN_DATA6 14
#define LCDCFG_PIN_DATA7 16
#define LCDCFG_PIN_E 20
#define LCDCFG_PIN_RS 22
#define LCDCFG_PINGPIO_DATA4 5
#define LCDCFG_PINGPIO_DATA5 6
#define LCDCFG_PINGPIO_DATA6 7
#define LCDCFG_PINGPIO_DATA7 8
#define LCDCFG_PINGPIO_E 10
#define LCDCFG_PINGPIO_RS 11
#define LCDCFG_OUTPUT 1

char *int2str( int val, char * buffer );
char *double2str( double dval, char * buffer, int decimals);

void LCD_write4bits( bool rs, int data4 ){

	if (rs)
		LPC_GPIO2 -> FIOSET = 1 << LCDCFG_PINGPIO_RS;
	else
		LPC_GPIO2 -> FIOCLR = 1 << LCDCFG_PINGPIO_RS;
	LPC_GPIO2 -> FIOSET = 1 << LCDCFG_PINGPIO_E;
	LPC_GPIO2 -> FIOCLR = 0x0f << LCDCFG_PINGPIO_DATA4; //Clear data4
	LPC_GPIO2 -> FIOSET = (data4&15) << LCDCFG_PINGPIO_DATA4; //Write data4
	WAIT_ChronoUs( 100 );

	LPC_GPIO2 -> FIOCLR = 1 << LCDCFG_PINGPIO_E;
	WAIT_ChronoUs( 100 );

}

void LCD_write8bits( bool rs, int data8 ){
	LCD_write4bits( rs, data8>>4 );
	LCD_write4bits( rs, data8 );
}

void LCD_writeCMD(int cmd){
	LCD_write8bits(0, cmd);
}

void LCD_writeData(int data){
	LCD_write8bits(1, data);
}

/*************************/

void LCDText_WriteChar(char ch){
	LCD_writeData( ch );
}

void LCDText_WriteString(char *str){
	int j = 1;
	for(int i = 0; i < strlen(str); i++){
		if( i == 16){
			++j;
			LCDText_Locate(j, 0);
		}
		LCDText_WriteChar(str[i]);
	}
}

void LCDText_Locate(int row, int column){
	if( row%2 == 1)
		LCD_writeCMD( 0x80 ); 		//Force cursor to beginning ( 1st line)
	else
		LCD_writeCMD( 0xC0 ); 		//Force cursor to beginning ( 2nd line)
	for( int i = 0; i < column; i++)
		LCD_writeCMD( 0x14 ); 		//shift cursor position to right
}


void LCDText_Clear(void){
	LCD_writeCMD( 0x01 );
	WAIT_Ms(5);
}

void LCDText_Cursor(bool on){
	if(on)
		LCD_writeCMD( 0xE );
	else
		LCD_writeCMD( 0xC );
}

void LCDText_WriteRightArrow(){
	LCDText_WriteChar(0b01111110);
}


void LCDText_Init(void){

	LPC_PINCON -> PINSEL4 &= ~((LCDCFG_NOT_GPIO << LCDCFG_PIN_DATA4) | (LCDCFG_NOT_GPIO << LCDCFG_PIN_DATA5) |
			(LCDCFG_NOT_GPIO << LCDCFG_PIN_DATA6) | (LCDCFG_NOT_GPIO << LCDCFG_PIN_DATA7) |
			(LCDCFG_NOT_GPIO << LCDCFG_PIN_E) | (LCDCFG_NOT_GPIO << LCDCFG_PIN_RS));

	LPC_GPIO2 -> FIODIR |= ((LCDCFG_OUTPUT << LCDCFG_PINGPIO_DATA4) | (LCDCFG_OUTPUT << LCDCFG_PINGPIO_DATA5) |
			(LCDCFG_OUTPUT << LCDCFG_PINGPIO_DATA6) | (LCDCFG_OUTPUT << LCDCFG_PINGPIO_DATA7) |
			(LCDCFG_OUTPUT << LCDCFG_PINGPIO_E) | (LCDCFG_OUTPUT << LCDCFG_PINGPIO_RS));


	WAIT_Ms(16);
	LCD_write4bits( 0, 3 );
	WAIT_Ms(5);
	LCD_write4bits( 0, 3 );
	WAIT_ChronoUs(110);
	LCD_write4bits( 0, 3 );

	LCD_write4bits( 0, 2 );
	LCD_writeCMD(0x28); //function set
	LCD_writeCMD(0xF); //display on/off control
	LCD_writeCMD(0x1); //Clears display
	WAIT_Ms(5);
	LCD_writeCMD(0x6); //entry mode set


}

void LCDText_Printf( char *fmt, ...){ //"The C Programming Language" by Ritchie and Kernighan Adapted
   va_list ap; // points to each unnamed arg in turn
   char *p, *sval, cval, buffer[10];
   int ival, decimals;
   double dval;

   va_start(ap, fmt); // make ap point to 1st unnamed arg

   LCDText_Locate(1, 0);
   int line = 1;

   for (p = fmt; *p; p++){
	   if (*p == '\n'){
		   LCDText_Locate(line%2 + 1, 0);
		   line++;
		   continue;
	   }
	   if (*p != '%' && *p != 'º'){
		   LCDText_WriteChar(*p);
		   continue;
	   }
	   if ( *p == 'º'){
		   LCDText_WriteChar(0b11011111);	//código do LCD para o carater º
		   continue;
	   }
	   switch (*++p){
	   case '2':
		   ival = va_arg(ap, int);
		   if( ival < 10)
			   LCDText_WriteChar( '0' );
		   LCDText_WriteString( int2str( ival, buffer ) );
		   p++;
		   break;
	   case 'd':
		   ival = va_arg(ap, int);
		   LCDText_WriteString( int2str( ival, buffer ) );
		   break;
	   case '.':
		   decimals = *++p - '0';
		   dval = va_arg(ap, double);
		   LCDText_WriteString( double2str( dval, buffer, decimals));
		   p++;
		   break;
	   case 'f':
		   dval = va_arg(ap, double);
		   LCDText_WriteString( double2str( dval, buffer, 4) );
		   break;
	   case 'c':
		   cval = va_arg(ap, int);
		   LCDText_WriteChar(cval );
		   break;
	   case 's':
		   sval = va_arg(ap, char *);
		   LCDText_WriteString(sval);
		   /*for (sval = va_arg(ap, char *); *sval; sval++ )
			   LCDText_WriteChar( *sval );*/
		   break;
	   default:
		   LCDText_WriteChar(*p);
		   break;
	   }
   }
   va_end(ap); // clean up when done

}

void LCDText_SPrintf(char *str, int len, char *fmt, ...){ //"The C Programming Language" by Ritchie and Kernighan Adapted
   va_list ap; // points to each unnamed arg in turn
   char *p, *sval, cval, buffer[10];
   int ival, decimals;
   double dval;

   va_start(ap, fmt); // make ap point to 1st unnamed arg

   LCDText_Locate(1, 0);
   int line = 1;

   for (p = fmt; *p; p++){
	   if (*p == '\n'){
		   LCDText_Locate(line%2 + 1, 0);
		   line++;
		   continue;
	   }
	   if (*p != '%' && *p != 'º'){
		   LCDText_WriteChar(*p);
		   continue;
	   }
	   if ( *p == 'º'){
		   LCDText_WriteChar(0b11011111);	//código do LCD para o carater º
		   continue;
	   }
	   switch (*++p){
	   case '2':
		   ival = va_arg(ap, int);
		   if( ival < 10)
			   LCDText_WriteChar( '0' );
		   LCDText_WriteString( int2str( ival, buffer ) );
		   p++;
		   break;
	   case 'd':
		   ival = va_arg(ap, int);
		   LCDText_WriteString( int2str( ival, buffer ) );
		   break;
	   case '.':
		   decimals = *++p - '0';
		   dval = va_arg(ap, double);
		   LCDText_WriteString( double2str( dval, buffer, decimals));
		   p++;
		   break;
	   case 'f':
		   dval = va_arg(ap, double);
		   LCDText_WriteString( double2str( dval, buffer, 4) );
		   break;
	   case 'c':
		   cval = va_arg(ap, int);
		   LCDText_WriteChar(cval );
		   break;
	   case 's':
		   sval = va_arg(ap, char *);
		   LCDText_WriteString(sval);
		   /*for (sval = va_arg(ap, char *); *sval; sval++ )
			   LCDText_WriteChar( *sval );*/
		   break;
	   default:
		   LCDText_WriteChar(*p);
		   break;
	   }
   }
   va_end(ap); // clean up when done

}

char *int2str( int val, char * buffer ){
	itoa( val, buffer, 10);
	return buffer;
}

char *double2str( double dval, char array[], int decimals){
	char buffer[7];
	if( decimals > 4)
		decimals = 4;
	int signal;
	if( (signal=dval)<0) dval = dval*(-1);

	int multVal=1;
	for( int i = 0; i < decimals; i++){
		multVal *= 10;
	}
	int val = (int)(dval*multVal);

	int i = 0;
	for( ; i<decimals; i++){
		buffer[5-i] = '0' + val%10;
		val /= 10;
	}
	buffer[5-i]='.';
	for( i += 1; i<6; i++){
		buffer[5-i] = '0' + val%10;
		val /= 10;
	}
	buffer[6]='\0';
	for( int j = 0; buffer[0] == '0'; j++){
		for(i = 0; i<6; i++)
			buffer[i] = buffer[i+1];
		buffer[5-j]='\0';
	}
	for( int j = 0; j<7; j++){
		array[j] = buffer[j];
	}
	return array;
}































