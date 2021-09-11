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
	OPTION_REGbits.nWPUEN = 1;
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
	HisteresisBits.Byte_Histeresis = 0;
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
	if(Rele == 1)
		Umbral = Umbral_ON;
	else
		Umbral = Umbral_OFF;
	FlagIRBits.Prog = 1;
	Config_T1_IR();
	T1CONbits.TMR1ON = 1;
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 0;
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
	while(FlagIRBits.FinIR == 0 && Cont_Prog_IR < Tiempo_Programar_IR)
	{
		if(INTCONbits.TMR0IF == 1)
		{
			INTCONbits.TMR0IF = 0;
			Cont_Prog_IR++;
			CLRWDT();
		}
	}
	if(FlagIRBits.FinIR)
	{
		if(contador == 8 || contador == 16 || contador == 32)
		{
			Graba_Patron_IR();
			Rele = ~Rele;
			if(Rele == 1)
				Umbral = Umbral_ON;
			else
				Umbral = Umbral_OFF;
		}
	}
	Cont_Prog_IR = 0;
	FlagIRBits.START = 0;
	FlagIRBits.DATA = 0;
	FlagIRBits.FinIR = 0;
	FlagIRBits.Prog = 0;
	Cont=0;
	Config_T1_mTouch();
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