/* 
 * File:   Funciones.c
 * Author: APateti
 *
 * Created on 16 de julio de 2015, 03:30 PM
 */
/*
 *Se Definen las Funciones a Utilizar en el Programa Principal
*/
#include <xc.h>
#define	baud		12		//Baudios a 19200
#define	Desh_INT_Rx 	PIE1bits.RCIE = 0
#define	Hab_INT_Rx 		PIE1bits.RCIE = 1

//****************************************************************
//	Funcion que Configura el TIMER2 para scan de ver si se Rx IR
//	Entrada: Config_T2
//  T2 = (4/4MHz) * PreDiv * PosDiv * (PR2+1)
//  T2 = 16 mSeg
//	Con Post en 10 da 160 mseg
//****************************************************************
//
void Config_T2(void)
{
	PR2 = 249;
	T2CONbits.T2CKPS = 0b11;		//PreDiv 64
	T2CONbits.T2OUTPS = 0b0000;		//PosDiv 1
	//T2CONbits.T2OUTPS = 0b1001;		//PosDiv 10
	T2CONbits.TMR2ON = 0;
	TMR2 = 0;
}
//****************************************************************
//	Funcion que Configura el TIMER2 para scan de ver si se Rx IR
//	Entrada: Config_T2_FinIR
//  T2 = (4/4MHz) * PreDiv * PosDiv * (PR2+1)
//  T2 = 48 mSeg
//	Con Post en 10 da 160 mseg
//****************************************************************
//
void Config_T2_FinIR(void)
{
	PR2 = 249;
	T2CONbits.T2CKPS = 0b11;		//PreDiv 64
	T2CONbits.T2OUTPS = 0b00100;		//PosDiv 3
	//T2CONbits.T2OUTPS = 0b1001;		//PosDiv 10
	T2CONbits.TMR2ON = 0;
	TMR2 = 0;
}
//****************************************************************
//	Funcion que Configura el TIMER0 para scan de mTouch
//	Entrada: Config_T0
//****************************************************************
//
void	Config_T0(void)
{
	OPTION_REGbits.nWPUEN = 0;
	OPTION_REGbits.TMR0CS = 0;			//Envia Inicio de Protocolo
	OPTION_REGbits.TMR0SE = 0;			//0xAA:0x55
	OPTION_REGbits.PSA = 0b0;
	OPTION_REGbits.PS = 0b110;
	//
	TMR0 = 0;
}
//****************************************************************
//	Funcion que Configura el Timer 1
//	Entrada: Config_T1
//****************************************************************
//
void	Config_T1_IR(void)
{
	T1CONbits.TMR1CS = 0b00;	// CLK = Fosc/4
	T1CONbits.T1CKPS = 0b00;	// 
	T1CONbits.nT1SYNC = 0b1;		//
	T1CONbits.TMR1ON = 0b0;		//  
}
//****************************************************************
//	Funcion que Configura el Timer 1
//	Entrada: Config_T1
//****************************************************************
//
void	Config_T1_mTouch(void)
{
	T1CONbits.TMR1CS = 0b11;	// CLK = Fosc/4
	T1CONbits.T1CKPS = 0b00;	// 
	T1CONbits.nT1SYNC = 0b1;		//
	T1CONbits.TMR1ON = 0b1;		//  
	//
	T1GCONbits.TMR1GE = 1;
	T1GCONbits.T1GPOL = 0;
	T1GCONbits.T1GTM = 0;
	T1GCONbits.T1GSPM = 0;
	T1GCONbits.T1GVAL = 0;
	T1GCONbits.T1GSS = 0b01;
	//
	TMR1 = 0;
}
//void putUART( char c){
//  while ( PIR1bits.TXIF==0); //Espera a que se pueda Tx
//  TXREG = c;
//}
//void EnvUART(char *Datos)
//{
//	while(*Datos != 0)
//	{
//		char c;
////		c=*Datos
//		putUART(*Datos);
//		*Datos++;
//	}
//}
//
//****************************************************************
//	Funcion que Configura EL Modulo mTouch
//	Entrada: Config_CPS
//****************************************************************
//
void	Config_CPS(void)
{
	CPSCON1 = 3;
	//CPSCON0 = 0b10001101;
	CPSCON0bits.T0XCS = 0b0;
	CPSCON0bits.CPSRNG = 0b11;
	CPSCON0bits.CPSRM = 0b0;
	CPSCON0bits.CPSON = 0b1;
}

//****************************************************************
//	Funcion que Configura las Interrupciones
//	Entrada: Config_INT
//****************************************************************
//
void	Config_INT(void)
{
//	OPTION_REGbits.nWPUEN = 0;
	WPUAbits.WPUA2 = 1;
	OPTION_REGbits.INTEDG = 0;
	INTCONbits.INTF = 0;
	INTCONbits.INTE = 1;
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 1;
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
}
//****************************************************************
//	Funcion que Inicializa las Caliables utilizadas en el Programa
//	Entrada: Initial_Var
//****************************************************************
//
void Initial_Var(void)
{
	Cont = 0;
	BanderasBits.Byte_Banderas = 0;
//	HisteresisBits.Byte_Histeresis = 0;
	Cont_Prog_IR = 0;
}
//****************************************************************
//	Funcion que Compara si el dato Rx por IR es Igual al Patron
//	Entrada: Comparar_IR
//****************************************************************
//
char Comparar_IR(void)
{
	char i;
	FlagIRBits.Igual_Patron = 1;
	if(Patron_Cont == contador)
	{
		FlagIRBits.Igual_Patron = 1;
		//Led_R = ~Led_R;
	}
	else
		FlagIRBits.Igual_Patron = 0;
	if(Patron_Start < (Time_start-250) || Patron_Start > (Time_start+250))
		FlagIRBits.Igual_Patron = 0;
	for(i=0; i<Patron_Cont; i++)
	{
		if(Patron_Bits[i] < (Time_Bits[i]-100) || Patron_Bits[i] > (Time_Bits[i]+100))
			FlagIRBits.Igual_Patron = 0;
	}
	if(FlagIRBits.Igual_Patron == 1)
	{
		//Led_R = ~Led_R;
		return(1);
	}
	else
	{
	//	Led_R = ~Led_R;
		return(0);
	}
}
//****************************************************************
//	Funcion que Lee el Patron IR de la EEPROM
//    Retorna 1 si hay Patron Almacebado y 0 en caso Contrario
//	Entrada: Leer_Patron_IR
//****************************************************************
//
char Leer_Patron_IR(void)
{
	char i;
	char	Dato_L;		// 1 si Se Rx el Tiempo de START
	char	Dato_H;		// 1 si Finalizo de Rx el dato IR
	INTCONbits.GIE = 0;
	Dato_L = eeprom_read (0);
	if(Dato_L == 0xFF)
	{
		TMR0 = 0;
		TMR1 = 0;
		INTCONbits.TMR0IF = 0;
		INTCONbits.GIE = 1;
		return(0);
	}
	else
	{
		Patron_Cont = Dato_L;
		Dato_L = eeprom_read (2);
		Dato_H = eeprom_read (3);
		Patron_Start = (Dato_H<<8 | Dato_L);
		for(i=0;i<(Patron_Cont);i++)
		{
			Dato_L = eeprom_read ((i*2)+4);
			Dato_H = eeprom_read (((i*2)+4)+1);
			Patron_Bits[i] = (Dato_H<<8 | Dato_L);
		}
		TMR0 = 0;
		TMR1 = 0;
		INTCONbits.TMR0IF = 0;
		INTCONbits.GIE = 1;
		return(1);
	}
}

//****************************************************************
//	Funcion que Graba el Patron IR en la EEPROM
//	Entrada: Graba_Patron_IR
//****************************************************************
//
void Graba_Patron_IR(void)
{
	char i, Dato_L, Dato_H;
	INTCONbits.GIE = 0;
	Patron_Cont = contador;
	Patron_Start = Time_start;
	for(i=0; i<(contador); i++)
		Patron_Bits[i] = Time_Bits[i];
	eeprom_write(0, Patron_Cont);
	Dato_L = Patron_Start;
	Dato_H = Patron_Start>>8;
	eeprom_write(2, Dato_L);
	eeprom_write(3, Dato_H);
	for(i=0;i<(Patron_Cont);i++)
	{
		Dato_L = Patron_Bits[i];
		Dato_H = Patron_Bits[i]>>8;
		eeprom_write ((i*2)+4,Dato_L);
		eeprom_write (((i*2)+4)+1,Dato_H);
	}
	TMR0 = 0;
	TMR1 = 0;
	INTCONbits.TMR0IF = 0;
	INTCONbits.GIE = 1;
}

//****************************************************************
//	Funcion que Determina la Tecla IR presionada y la Guarda en
//    la EEPROM siendo esta la tecla configurada
//	Entrada: Programar_IR
//****************************************************************
//
void Programar_IR(void)
{
	Rele = ~Rele;
	FlagIRBits.Prog = 1;
	Config_T1_IR();
	T1CONbits.TMR1ON = 1;
	INTCONbits.TMR0IF = 0;
	//INTCONbits.TMR0IE = 0;
	//BanderasBits.Inicio_IR = 0;
	//BanderasBits.Ini_Start = 0;
	//BanderasBits.Fin_IR = 0;
	FlagIRBits.START = 0;
	FlagIRBits.DATA = 0;
	FlagIRBits.FinIR = 0;
//	Cont = 0;
	Time_start=0;
	CPSCON0 = 0;
	Cont_Prog_IR = 0;
	TMR0 = 0;
	//putUART("P");
	while(FlagIRBits.FinIR == 0 && Cont_Prog_IR < Tiempo_Programar_IR)
	{
		if(INTCONbits.TMR0IF == 1)
		{
			//INTCONbits.TMR0IF = 0;
			//Cont_Prog_IR++;
			CLRWDT();
		}
	}
	if(FlagIRBits.FinIR)
	{
		//putUART("p");
		if(contador == 8 || contador == 16 || contador == 32)
		{
			Graba_Patron_IR();
			Rele = ~Rele;
			INTCONbits.TMR0IE = 0;
		}
	}
	Cont_Prog_IR = 0;
	FlagIRBits.START = 0;
	FlagIRBits.DATA = 0;
	FlagIRBits.FinIR = 0;
	FlagIRBits.Prog = 0;
	Cont=0;
	//Config_T1_mTouch();
	TMR0 = 0;
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 1;
	//CPSCON0bits.CPSON = 1;
	Config_CPS();
	Conteo_Ini = 0;
	T2CONbits.TMR2ON = 0;
	//TMR2 = 0;
	//PIR1bits.TMR2IF = 0;
	//PIE1bits.TMR2IE = 0;
}
//Hex2BCD(unsigned int HEXADECIMAL)
//{
//	char i;
//	i=0;
//	Num_BCD[0] = 0;
//	Num_BCD[1] = 0;
//	Num_BCD[2] = 0;
//	Num_BCD[3] = 0;
//	Num_BCD[4] = 0;
//	while(HEXADECIMAL > 0)
//	{
//		Num_BCD[i] = HEXADECIMAL % 10;
//		HEXADECIMAL /=10;
//		i++; 
//	}
//}
//char HEX2ASCII(char c)
//{
//	char ASCII;
//	if(c < 0x0A)
//		ASCII = c + 0x30;
//	else
//		ASCII = c + 0x37;
//	return(ASCII);
//}
////****************************************************************
////	Funcion que Configura el Modulo UART para que solo Tx a 19200
////	Entrada: Ninguna
////****************************************************************
////
//void Initial_UART(void)
//{
//	//Se Configura los Baudios
//	SPBRG = baud;
//	APFCONbits.TXCKSEL = 0;			//Selecciona RC6 como TX
//	APFCONbits.RXDTSEL = 0;			//Selecciona RC7 como RX
//	//Se Configura el Registro TXSTA
//	TXSTAbits.CSRC = 0;				//Solo para Sincrono
//	TXSTAbits.TX9  = 0;				//No Tx 9no Bit
//	TXSTAbits.SYNC = 0;				//UART Asincrono
//	TXSTAbits.BRGH = 1;				//Ecuacion Alta Vel
//	TXSTAbits.TXEN = 1;				//Habilita Tx
//	//TXSTA = 0x24;
//	//Se Configura el Registro RCSTA
//	RCSTAbits.RX9  = 0;				//Rx solo 8 bits
//	RCSTAbits.SREN = 0;				//Rx solo para Sincrono
//	RCSTAbits.CREN = 0;				//DesHabilita Rx
//	RCSTAbits.RX9  = 0;				//Rx solo 8 bits
//	RCSTAbits.ADDEN = 0;			//Deshabilita Dir Esclavo
//	RCSTAbits.SPEN  = 1;			//Habilita Puerto Serial
//	PIR1bits.RCIF = 0;
//	//PIE1bits.RCIE = 0;
//	Desh_INT_Rx;
//	INTCONbits.PEIE = 1;
//	INTCONbits.GIE = 1;
//}
//
//void Env_Cod_IR(void)
//{
//	char i;
//	//printf("Datos IR: \n");
//	//printf("Cont IR: %d\n", Cont);
//	//printf("Start IR: %d\n", Time_start);
//	EnvUART("\n\rDatos IR: \n");
//	EnvUART("Cont: ");
//	Hex2BCD(contador);
//	putUART(HEX2ASCII(Num_BCD[1]));
//	putUART(HEX2ASCII(Num_BCD[0]));
//	putUART(CR);
//	putUART(LF);
//	EnvUART("\n\rStart IR: ");
//	Hex2BCD(Time_start);
//	putUART(HEX2ASCII(Num_BCD[3]));
//	putUART(HEX2ASCII(Num_BCD[2]));
//	putUART(HEX2ASCII(Num_BCD[1]));
//	putUART(HEX2ASCII(Num_BCD[0]));
//	putUART(CR);
//	putUART(LF);
//	//printf("Contador %d\n", i,Time_Bits[i]);
//	//printf("Cont: %2d",Cont);
//	//printf("%d\n", Time_start);
//	for(i=0; i<contador; i++)
//	{
//		EnvUART("Dato [");
//		Hex2BCD(i);
//		putUART(HEX2ASCII(Num_BCD[1]));
//		putUART(HEX2ASCII(Num_BCD[0]));
//		EnvUART("]: ");
//		Hex2BCD(Time_Bits[i]);
//		putUART(HEX2ASCII(Num_BCD[3]));
//		putUART(HEX2ASCII(Num_BCD[2]));
//		putUART(HEX2ASCII(Num_BCD[1]));
//		putUART(HEX2ASCII(Num_BCD[0]));
//		putUART(CR);
//		putUART(LF);
//		//printf("%d]: %d\n", i,Time_Bits[i]);
//	}
//		
//	//}
//}
//
//void	Tx_UART_Patron(void)
//{
//	char i;
////	printf("Datos Patron IR: \n");	
////	rintf("Cont Patron IR: %d\n", Patron_Cont);	
////	printf("Start Patron IR: %d\n", Patron_Start);	
////	for(i=0;i<Patron_Cont; i++)	
////	{
////		printf("Dato [%d]: %d\n", i,Patron_Bits[i]);
////	}
//	EnvUART("Cont Pat: ");
//	Hex2BCD(Patron_Cont);
//	putUART(HEX2ASCII(Num_BCD[1]));
//	putUART(HEX2ASCII(Num_BCD[0]));
//	putUART(CR);
//	putUART(LF);
//	EnvUART("\n\rStart P: ");
//	Hex2BCD(Patron_Start);
//	putUART(HEX2ASCII(Num_BCD[3]));
//	putUART(HEX2ASCII(Num_BCD[2]));
//	putUART(HEX2ASCII(Num_BCD[1]));
//	putUART(HEX2ASCII(Num_BCD[0]));
//	putUART(CR);
//	putUART(LF);
//	for(i=0; i<Patron_Cont; i++)
//	{
//		EnvUART("Dato [");
//		Hex2BCD(i);
//		putUART(HEX2ASCII(Num_BCD[1]));
//		putUART(HEX2ASCII(Num_BCD[0]));
//		EnvUART("]: ");
//		Hex2BCD(Patron_Bits[i]);
//		putUART(HEX2ASCII(Num_BCD[3]));
//		putUART(HEX2ASCII(Num_BCD[2]));
//		putUART(HEX2ASCII(Num_BCD[1]));
//		putUART(HEX2ASCII(Num_BCD[0]));
//		putUART(CR);
//		putUART(LF);
//	}
//}