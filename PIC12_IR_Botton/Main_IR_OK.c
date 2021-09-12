//***************************************************/
/*Programa para el que esta funcionando el el cuarto*/
//***************************************************/
#include <stdio.h>		
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>


#define Num_P   16
#define Div		4
#define Lib_Tec 10
#define Inf_Tec 40

#define LED_ON   0
#define LED_OFF  1
#define RELE_ON   1
#define RELE_OFF  0

//#define	Umbral_ST	3000	//Umbral Sin tapa
//#define	Umbral_CT	350		//Umbral Con tapa
//#define	Umbral_CT	600		//Umbral Con tapa
//#define	Histereis	300		//Umbral Con tapa
//#define	Umbral_CT	600		//Umbral Con tapa Vieja
//#define	Histereis	250		//Histeresis Con tapa Vieja
//#define	Histereis_Prom	100		//histeresis Con tapa nueva para hacer Promedio
//
//#define	Umbral_CT_N	900		//Umbral Con tapa nueva
//#define	Histereis_N	450		//Histeresis Con tapa nueva
//#define	Histereis_Prom_N	200		//histeresis Con tapa Vieja para hacer Promedio
//
//Con la tapa maaas nueva, con nueva figura posterior
//#define	Umbral_CT_N	550		//Umbral Con tapa nueva
//#define	Histereis_N	250		//Histeresis Con tapa nueva
//#define	Histereis_Prom_N	100		//histeresis Con tapa Vieja para hacer Promedio
//#define	Umbral_OFF	1150
//#define	Umbral_ON	1250
//#define	Hist_Prog	50

#define	Max_Bits_IR	32
#define	Tiempo_Programar_IR 125	//Se cuentan 125 Interrupciones de Timer 0
								//A 32 ms genera 4 Seg de espera
#define	CR	0x0D
#define	LF	0x0A
//#define Total_Bits  32
#define Uno_Bit_IR 0x0001
//Datos Leidos de la ultima tecla IR presionada
unsigned int Tiempo_Bit;
unsigned int Time_start;
unsigned int Time_Bits[32];
//Datos Patron IR a ser comparados. Patron
unsigned int Patron_Start;
unsigned int Patron_Bits[32];
char		 Patron_Cont;
char		Cont_Prog_IR;
char		Cont_Prog_LED;
char		Cont_Tec_Lib;
//unsigned char Carac, Total_Bits_IR, Fin_T0, ind_prom,Conteo_Ini;
unsigned char Cont, contador, Conteo_Ini;

#include "Configuration_bit.c"

#define TRIS_Rele	TRISAbits.TRISA1
#define Rele			LATAbits.LATA1
//Define los Leds que indicaran el estado o posibles fallas delmodulo
//#define TRIS_Led_V		TRISAbits.TRISA0
//#define Led_V			LATAbits.LATA0

#define TRIS_Led_R		TRISAbits.TRISA5
#define Led_R			LATAbits.LATA5

#define TRIS_Led_Status		TRISAbits.TRISA4
#define Led_Status			LATAbits.LATA4

#define TRIS_Sw		TRISAbits.TRISA3
#define Sw			PORTAbits.RA3
//char 			Num_BCD[6];
#include "System.h"
#include "User.h"
#include "Funciones.c"

int main(void) 
{
	char i;
	OSCCON = 0b01101000;
	if(PCONbits.nPOR == 0)
	{
		Rele = RELE_ON;
		CLRWDT();
		PCONbits.nPOR = 1;
	}
	TRIS_Rele = 0;
	TRIS_Sw = 1;
	OPTION_REGbits.nWPUEN = 0;
//Leds de Pruebas
//	Led_V = 1;
	if(Rele)
		Led_R = LED_ON;
	else
		Led_R = LED_OFF;
//	TRIS_Led_V = 0;
	TRIS_Led_R = 0;
	TRIS_Led_Status = 0;
	Led_Status = LED_OFF;

//
//	Umbral = Umbral_OFF;
//	WDTCON = 0b00011011;	//Habilita WDT a 8 Seg 
	ANSELA = 0;
	APFCON = 0;
	Conteo_Ini = 0;
	Cont_Tec_Lib = 0;
	Config_T0();
	Config_INT();
	Initial_Var();
	Config_T1_IR();
	Config_T2();
	//Initial_UART();
	Cont_Prog_LED = 0;

	Patron_Start = 0;
	for(i=0; i<32; i++)
		Patron_Bits[i] = 0;
	if(Leer_Patron_IR() == 0)
		Patron_Cont = 0;

	BanderasBits.Tec_1V = 1;
	INTCONbits.TMR0IE = 0;
	PIE1bits.TMR2IE = 0;
//	putUART('P');
//	putUART('r');
//	putUART('u');
//	putUART('e');
//	putUART('b');
//	putUART('a');
//	putUART(CR);
//	putUART(LF);
//	Tx_UART_Patron();
//	EnvUART("Datos IR\n");
	while(1)
	{
 
		if(!Sw)
		{
			__delay_ms(10);
			//Cont_Prog_IR = 0;
			if(!Sw)
			{
				if(BanderasBits.Tec_1V)
				{
					BanderasBits.Tec_1V = 0;
					Cont_Prog_LED = 0;
					Rele = ~Rele;
					if(Rele)
						Led_R = LED_ON;
					else
						Led_R = LED_OFF;
					INTCONbits.TMR0IF = 0;
					INTCONbits.TMR0IE = 1;
				}
				if(Cont_Prog_IR >= 50)
				{
					Led_R = ~Led_R;
					Programar_IR();
				}
			}
		}
		else
		{
			Cont_Prog_IR = 0;
			BanderasBits.Tec_1V = 1;
			INTCONbits.TMR0IE = 0;
		}
		if(FlagIRBits.FinIR)
		{
			Led_Status = LED_OFF;
			if(Patron_Cont != 0)
			{
				//Led_R = ~Led_R;
				if(Comparar_IR()==1)
					Rele = ~Rele;
				if(Rele)
					Led_R = LED_ON;
				else
					Led_R = LED_OFF;
			}
//			BanderasBits.Fin_IR = 0;
			FlagIRBits.START = 0;
			FlagIRBits.DATA = 0;
			FlagIRBits.FinIR = 0;
			Cont=0;
			//Config_T1_mTouch();
			TMR0 = 0;
			INTCONbits.TMR0IF = 0;
			INTCONbits.TMR0IE = 0;
			//CPSCON0bits.CPSON = 1;
			//Config_CPS();
			Conteo_Ini = 0;
			//T2CONbits.TMR2ON = 0;
			//TMR2 = 0;
			//PIR1bits.TMR2IF = 0;
			//PIE1bits.TMR2IE = 0;
//			Led_R = ~Led_R;
			//Env_Cod_IR();
			//putUART("E");
			INTCONbits.INTE = 1;
		}
		CLRWDT();
		
	}
}
// Interrupcion de TMR0 e INT0
// TMR0 interumpe cada 32 mSeg para el mTouch
// INT0 interrumpe con el sensor IR en cada Flanco de Bajada
void interrupt isr(void)
{

	if(INTCONbits.TMR0IF == 1 && INTCONbits.TMR0IE == 1)
	{
		INTCONbits.TMR0IF = 0;
		//putUART('t');
		Cont_Prog_IR++;
		if(FlagIRBits.Prog)
		{
			Cont_Prog_LED++;
			if(Cont_Prog_LED >= 10)
			{
				Cont_Prog_LED = 0;
				Led_R = ~Led_R;
			}
		}
	}	
   if(INTCONbits.INTE == 1 && INTCONbits.INTF == 1)
	{
		//putUART('I');
		Tiempo_Bit = TMR1;
		TMR1 = 0;
		TMR2 = 0;
		INTCONbits.INTF = 0;
		if(FlagIRBits.DATA)						//Se esta Rx los Bits IR?
		{
			Time_Bits[Cont++]=Tiempo_Bit;
			TMR2 = 0;
			//putUART('d');
			//putUART('F');
			if(Tiempo_Bit > (Time_start+500))
			{
				FlagIRBits.FinIR = 1;
				contador = Cont-1;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
				Led_Status = LED_OFF;
				//putUART('f');
			}
			else if(Cont >= Max_Bits_IR)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
				Led_Status = LED_OFF;
				//putUART('a');
			}
			else if(!FlagIRBits.Prog)
			{
				if(Cont == Patron_Cont)
				{
					FlagIRBits.FinIR = 1;
					contador = Cont;
					Config_T2_FinIR();
					T2CONbits.TMR2ON = 1;
					INTCONbits.INTE = 0;
					FlagIRBits.Sleep_IR = 1;
					Led_Status = LED_OFF;
					//putUART('F');
				}
			}
			//putUART('d');
		}
		else if(FlagIRBits.START == 1)
		{
				FlagIRBits.START = 0;
				FlagIRBits.DATA = 1;
				FlagIRBits.FinIR = 0;
				Cont = 0;
				Time_start = Tiempo_Bit;
				TMR2 = 0;
//				if(FlagIRBits.Prog == 0)
//				{
//					if(Patron_Cont)
//					{
//						if(Patron_Start < (Time_start-550) || Patron_Start > (Time_start+550))
//						{
//							FlagIRBits.START = 0;
//							FlagIRBits.DATA = 0;
//							FlagIRBits.FinIR = 0;
//							Cont=0;
//							Config_T1_mTouch();
//							TMR0 = 0;
//							INTCONbits.TMR0IF = 0;
//							INTCONbits.TMR0IE = 1;
//							//CPSCON0bits.CPSON = 1;
//							Config_CPS();
//							Conteo_Ini = 0;
//							TMR2 = 0;
//							PIR1bits.TMR2IF = 0;
//							//PIE1bits.TMR2IE = 0;
//							Config_T2_FinIR();
//							T2CONbits.TMR2ON = 1;
//							INTCONbits.INTE = 0;
//							FlagIRBits.Sleep_IR = 1;
//						}
//					}
//				}
				//putUART('S');
		}
		else
		{
			//Config_T1_IR();
			T1CONbits.TMR1ON = 1;
			INTCONbits.TMR0IF = 0;
			INTCONbits.TMR0IE = 0;
			FlagIRBits.DATA = 0;
			FlagIRBits.START = 1;
			FlagIRBits.Sleep_IR = 0;
			Cont = 0;
			Time_start=0;
			CPSCON0 = 0;
			TMR2 = 0;
			//Config_T2();
			//T2CONbits.TMR2ON = 1;
			TMR2 = 0;
			PIR1bits.TMR2IF = 0;
			PIE1bits.TMR2IE = 1;
			T2CONbits.TMR2ON = 1;
			//Estaus LED = ON
			Led_Status = LED_ON;
			//putUART('i');
		}
	}
	if(PIR1bits.TMR2IF == 1 && PIE1bits.TMR2IE == 1)
	{
		//putUART('T');
		if(FlagIRBits.Sleep_IR == 1)
		{
			//Led_R = ~Led_R;
			T2CONbits.TMR2ON = 0;
			TMR2 = 0;
			PIR1bits.TMR2IF = 0;
			PIE1bits.TMR2IE = 0;
			INTCONbits.INTF = 0;
			INTCONbits.INTE = 1;
			FlagIRBits.Sleep_IR = 0;
		}
		else
		{
			if(Cont >= Max_Bits_IR)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
				Led_Status = LED_OFF;
			}
			else if(FlagIRBits.Prog == 1)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
				Led_Status = LED_OFF;
			}
			else if(Cont == Patron_Cont)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
				Led_Status = LED_OFF;
			}
			else
			{
				//Config_T1_mTouch();
				//Config_CPS();
				//TMR0 = 0;
				//INTCONbits.TMR0IF = 0;
				//INTCONbits.TMR0IE = 1;
				//CPSCON0bits.CPSON = 1;
				FlagIRBits.START = 0;
				FlagIRBits.DATA = 0;
				FlagIRBits.FinIR = 0;
				FlagIRBits.Sleep_IR = 1;
				Cont = 0;
				Conteo_Ini = 0;
				TMR2 = 0;
				T2CONbits.TMR2ON = 0;
				PIR1bits.TMR2IF = 0;
				PIE1bits.TMR2IE = 0;
				Led_Status = LED_OFF;
			}

		}
	}
}