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

#define	Umbral_ST	3000	//Umbral Sin tapa
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
#define	Umbral_CT_N	550		//Umbral Con tapa nueva
#define	Histereis_N	250		//Histeresis Con tapa nueva
#define	Histereis_Prom_N	100		//histeresis Con tapa Vieja para hacer Promedio
#define	Umbral_OFF	1000
#define	Umbral_ON	1050
#define	Hist_Prog	50

#define	Max_Bits_IR	32
#define	Tiempo_Programar_IR 125	//Se cuentan 125 Interrupciones de Timer 0
								//A 32 ms genera 4 Seg de espera

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
char		Cont_Tec_Lib;
//unsigned char Carac, Total_Bits_IR, Fin_T0, ind_prom,Conteo_Ini;
unsigned char Cont, contador, Conteo_Ini;
unsigned int  Promedio[Num_P];
unsigned int  Tiempo_mTouch, Promedio_16;
unsigned int	Umbral;
unsigned long int Promedio_32;



//unsigned int Tiempo_Bit, Dato_IR_16;
//unsigned long int Dato_IR;
//char i,j,Cont, contador;

#include "Configuration_bit.c"

#define TRIS_Rele	TRISAbits.TRISA1
#define Rele			LATAbits.LATA1
//Define los Leds que indicaran el estado o posibles fallas delmodulo
//#define TRIS_Led_V		TRISAbits.TRISA0
//#define Led_V			LATAbits.LATA0

#define TRIS_Led_R		TRISAbits.TRISA5
#define Led_R			LATAbits.LATA5

//#define TRIS_Led_R		TRISAbits.TRISA5
//#define Led_R			LATAbits.LATA5

#include "System.h"
#include "User.h"
#include "Funciones.c"

int main(void) 
{
	char i;
	OSCCON = 0b01101000;
	if(PCONbits.nPOR == 0)
	{
		Rele = 0;
		CLRWDT();
		PCONbits.nPOR = 1;
	}
	TRIS_Rele = 0;
//Leds de Pruebas
//	Led_V = 1;
	Led_R = 1;
//	TRIS_Led_V = 0;
	TRIS_Led_R = 0;

//
	Umbral = Umbral_OFF;
//	WDTCON = 0b00011011;	//Habilita WDT a 8 Seg 
	ANSELA = 0;
	APFCON = 0;
	Conteo_Ini = 0;
	Cont_Tec_Lib = 0;
	Config_T1_mTouch();
	Config_T0();
	Config_INT();
	Initial_Var();
	Config_CPS();
	Config_T2();
//	Led_V = 1;
//	Led_R = 1;
//	TRIS_Led_V = 0;
//	TRIS_Led_R = 0;
	for(i=0;i<Num_P;i++)
	{
		Promedio[i]=0;
	}
//Inicializa Patron de IR
	Patron_Start = 0;
	for(i=0; i<32; i++)
		Patron_Bits[i] = 0;
	if(Leer_Patron_IR() == 0)
		Patron_Cont = 0;
	//Probar Led

//	__delay_ms(500);
//	Rele = ~Rele;
//	__delay_ms(500);
//	Rele = ~Rele;
	while(1)
	{
 		if(BanderasBits.Fin_T0 == 1)
		{
			BanderasBits.Fin_T0 = 0;
			if(BanderasBits.Tec_Pres == 1)
			{
				if(BanderasBits.Tec_Lib == 0)
				{
					Cont_Prog_IR++;
				}
				if(BanderasBits.Tec_1V == 1)
				{
					Rele = ~Rele;
					BanderasBits.Tec_1V = 0;
					BanderasBits.Tec_Lib = 0;
					Cont_Prog_IR = 0;
				}
				if(Rele == 1)
				{
					Umbral = Umbral_ON;
					Led_R = 0;
				}
				else
				{
					Umbral = Umbral_ON;
					Led_R = 1;
				}
			}
			if(Cont_Prog_IR > Tiempo_Programar_IR)
			{
				Cont_Prog_IR = 0;
				Programar_IR();
			}
		}
		if(FlagIRBits.FinIR)
		{
			if(Patron_Cont != 0)
			{
				//Led_V = ~Led_V;
				if(Comparar_IR()==1)
					Rele = ~Rele;
				if(Rele == 1)
				{
					Umbral = Umbral_ON;
					Led_R = 0;
				}
				else
				{
					Umbral = Umbral_OFF;
					Led_R = 1;
				}
			}
//			BanderasBits.Fin_IR = 0;
			FlagIRBits.START = 0;
			FlagIRBits.DATA = 0;
			FlagIRBits.FinIR = 0;
			Cont=0;
			Config_T1_mTouch();
			TMR0 = 0;
			INTCONbits.TMR0IF = 0;
			INTCONbits.TMR0IE = 1;
			//CPSCON0bits.CPSON = 1;
			Config_CPS();
			Conteo_Ini = 0;
			//T2CONbits.TMR2ON = 0;
			//TMR2 = 0;
			//PIR1bits.TMR2IF = 0;
			//PIE1bits.TMR2IE = 0;
//			Led_R = ~Led_R;
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
		char i;
		Tiempo_mTouch = TMR1;
		TMR1 = 0;
		TMR0 = 0;
		//TMR2 = 0;
		//Led_V = ~Led_V;
		if(Conteo_Ini < (Num_P))
		{
			for(i=0;i<Num_P-1;i++)
			{
				Promedio[i]=Promedio[i+1];
			}
			Promedio[Num_P-1]= Tiempo_mTouch;
			Promedio_32 = 0;
			for(i=0;i<Num_P;i++)
			{
				Promedio_32=Promedio[i]+Promedio_32;
			}
			Promedio_16=Promedio_32>>Div;
			Conteo_Ini++;
			HisteresisBits.Histe_Prom = 1;
			HisteresisBits.Activ_Prom = 0;
		}
		else
		{
			if(Tiempo_mTouch < (Promedio_16-Umbral))
			{
				BanderasBits.Tec_Pres = 1;
				//BanderasBits.Tec_Lib = 0;
				if(BanderasBits.Tec_Lib == 1)
					BanderasBits.Tec_1V = 1;
				Cont_Tec_Lib = 0;
				HisteresisBits.Activ_Prom = 1;
				HisteresisBits.Histe_Prom = 0;
			}
			else if((Tiempo_mTouch > (Promedio_16-Umbral)) && (Tiempo_mTouch < (Promedio_16-(Umbral-Hist_Prog))))
			{
				if(BanderasBits.Tec_Lib == 0)
				{
					BanderasBits.Tec_Pres = 1;
				}
				else
					BanderasBits.Tec_Pres = 0;
			}
			else if((Tiempo_mTouch > (Promedio_16-(Umbral-Hist_Prog))) && (Tiempo_mTouch < (Promedio_16-Histereis_N)))
			{
				BanderasBits.Tec_Pres = 0;
				BanderasBits.Tec_Lib = 1;
			}
			//else if(Tiempo_mTouch > (Promedio_16-(Umbral_CT_N+Histereis_N)))
			else if((Tiempo_mTouch > (Promedio_16-Histereis_N)) && (Tiempo_mTouch < (Promedio_16-Histereis_Prom_N)))
			{
				BanderasBits.Tec_Pres = 0;
				BanderasBits.Tec_Lib = 1;
				HisteresisBits.Histe_Prom = 0;
				//BanderasBits.Tec_1V = 1;
				Cont_Tec_Lib++;
				if(Cont_Tec_Lib > Inf_Tec)
				{
					Conteo_Ini = 0;
					Cont_Tec_Lib = 0;
				}
			}
			else//(Tiempo_mTouch >(Promedio_16-(Histereis_Prom_N)))
			{
				HisteresisBits.Histe_Prom = 1;
				BanderasBits.Tec_Lib = 1;
				BanderasBits.Tec_Pres = 0;
				Cont_Tec_Lib = 0;
				if(HisteresisBits.Activ_Prom == 1)
					Conteo_Ini = 0;
			}
			if(HisteresisBits.Histe_Prom == 1)
			{
				HisteresisBits.Histe_Prom = 0;
				for(i=0;i<Num_P-1;i++)
				{
					Promedio[i]=Promedio[i+1];
				}
				Promedio[Num_P-1]= Tiempo_mTouch;
				Promedio_32 = 0;
				for(i=0;i<Num_P;i++)
				{
					Promedio_32=Promedio[i]+Promedio_32;
					}
				Promedio_16=Promedio_32>>Div;
			}
		}
		INTCONbits.TMR0IF = 0;
		BanderasBits.Fin_T0 = 1;
	}
   if(INTCONbits.INTE == 1 && INTCONbits.INTF == 1)
	{
		Tiempo_Bit = TMR1;
		TMR1 = 0;
		TMR2 = 0;
		INTCONbits.INTF = 0;
		if(FlagIRBits.DATA)						//Se esta Rx los Bits IR?
		{
			Time_Bits[Cont++]=Tiempo_Bit;
			TMR2 = 0;
			if(Tiempo_Bit > (Time_start+500))
			{
				FlagIRBits.FinIR = 1;
				contador = Cont-1;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
			}
			else if(Cont == Max_Bits_IR)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
			}
			else if(Cont == Patron_Cont)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
			}
		}
		else if(FlagIRBits.START == 1)
		{
				FlagIRBits.START = 0;
				FlagIRBits.DATA = 1;
				FlagIRBits.FinIR = 0;
				Cont = 0;
				Time_start = Tiempo_Bit;
				TMR2 = 0;
				if(FlagIRBits.Prog == 0)
				{
					if(Patron_Cont)
					{
						if(Patron_Start < (Time_start-550) || Patron_Start > (Time_start+550))
						{
							FlagIRBits.START = 0;
							FlagIRBits.DATA = 0;
							FlagIRBits.FinIR = 0;
							Cont=0;
							Config_T1_mTouch();
							TMR0 = 0;
							INTCONbits.TMR0IF = 0;
							INTCONbits.TMR0IE = 1;
							//CPSCON0bits.CPSON = 1;
							Config_CPS();
							Conteo_Ini = 0;
							TMR2 = 0;
							PIR1bits.TMR2IF = 0;
							//PIE1bits.TMR2IE = 0;
							Config_T2_FinIR();
							T2CONbits.TMR2ON = 1;
							INTCONbits.INTE = 0;
							FlagIRBits.Sleep_IR = 1;
						}
					}
				}
		}
		else
		{
			Config_T1_IR();
			T1CONbits.TMR1ON = 1;
			INTCONbits.TMR0IF = 0;
			INTCONbits.TMR0IE = 0;
			FlagIRBits.DATA = 0;
			FlagIRBits.START = 1;
			Cont = 0;
			Time_start=0;
			CPSCON0 = 0;
			TMR2 = 0;
			Config_T2();
			T2CONbits.TMR2ON = 1;
			PIR1bits.TMR2IF = 0;
			PIE1bits.TMR2IE = 1;
			T2CONbits.TMR2ON = 1;
		}
	}
	if(PIR1bits.TMR2IF == 1)
	{
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
			if(Cont == Max_Bits_IR)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
			}
			else if(FlagIRBits.Prog == 1)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
			}
			else if(Cont == Patron_Cont)
			{
				FlagIRBits.FinIR = 1;
				contador = Cont;
				Config_T2_FinIR();
				T2CONbits.TMR2ON = 1;
				INTCONbits.INTE = 0;
				FlagIRBits.Sleep_IR = 1;
			}
			else
			{
				Config_T1_mTouch();
				Config_CPS();
				TMR0 = 0;
				INTCONbits.TMR0IF = 0;
				INTCONbits.TMR0IE = 1;
				//CPSCON0bits.CPSON = 1;
				FlagIRBits.START = 0;
				FlagIRBits.DATA = 0;
				FlagIRBits.FinIR = 0;
				Cont = 0;
				Conteo_Ini = 0;
				TMR2 = 0;
				T2CONbits.TMR2ON = 0;
				PIR1bits.TMR2IF = 0;
				PIE1bits.TMR2IE = 0;
			}

		}
	}
}