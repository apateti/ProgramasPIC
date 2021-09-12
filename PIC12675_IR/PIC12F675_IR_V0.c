	      #include <12f675.h>
//Los fusibles son estos, vienen en el archivo 12F675.h
//////// Fuses: LP,XT,HS,EC_IO,NOWDT,WDT,CPD,NOCPD,PROTECT,NOPROTECT,NOMCLR
//////// Fuses: MCLR,PUT,NOPUT,INTRC_IO,INTRC,RC_IO,RC,BROWNOUT,NOBROWNOUT
////////
      #fuses NOWDT,NOPROTECT,BROWNOUT,NOMCLR,INTRC_IO
      #use delay(clock=4000000)


      #include <Reg_12F675.h>   //Registros del PIC16F690
      #include <Teclas_CR.h>   	//Registros para Definir los codigos de cada 
      							//tecla del Control Remoto
      
    //#define Led_Verde    PIN_B4
    //#define TRIS_Led_Verde    TRISB.RB4
    #define Relay        PIN_A5
    //#define Led_Rojo     PIN_C0
    //#BYTE trisa = 0x85
    
    #define TRIS_Relay        TRISIO.TRISIO5
    //#define TRIS_Led_Verde    TRISA.RA0
    //#define TRIS_Led_Rojo     TRISC.RC0
//
//El Start dura aprox. 13.5 mS, 
//Si el TMR1 se incrementa cada 1 uS deben pasar 13500 ticks del TMR1
// Por lo tanto se compara el start con 13000 y 14000 Ticks del TMR1
const int16 Tiemp_Start_Min = 13000;
//#define Tiemp_Start_Min 13000
const int16 Tiemp_Start_Max = 14000;
//#define Tiemp_Start_Max 14000
//
//El Bit 0 dura aprox. 2.7 mS, 
//Si el TMR1 se incrementa cada 1 uS deben pasar 2700 ticks del TMR1
// Por lo tanto se compara el ´0´ con 2000 y 3000 Ticks del TMR1
const int16 Tiemp_0_Min = 2000;
//#define Tiemp_0_Min 2000
const int16 Tiemp_0_Max = 3000;
//#define Tiemp_0_Max 3000
//
//El Bit 1 dura aprox. 4.9 mS, 
//Si el TMR1 se incrementa cada 1 uS deben pasar 4900 ticks del TMR1
// Por lo tanto se compara el ´1´ con 4000 y 5000 Ticks del TMR1
const int16 Tiemp_1_Min = 4200;
//#define Tiemp_1_Min 4200
const int16 Tiemp_1_Max = 5800;
//#define Tiemp_1_Max 5800
   
//const char Total_Bits = 16; 
#define Total_Bits  16

BYTE i,j,cont;
char	compara;
int16 Tiempo_Bit, Bits_IR;
#define Uno_Bit_IR 0x8000

void clrear_Buffer(void)
{
	Bits_IR = 0;
}

 //Interrupcion Externa por el pin GP2
 //Esto es cuando ocurre una captura del Pin CCP1 por flanco bajada
 #INT_EXT
void Externa_interrupt()
{
	
	Tiempo_Bit = get_timer1();
	set_timer1(0);
	if(Banderas.Inicio_IR)
	{
		//Ver si se Rx el Bit 0
		if(Tiempo_Bit > Tiemp_0_Min && Tiempo_Bit < Tiemp_0_Max)
		{
			Bits_IR>>=1;
			cont++;
		}
		//Ver si se Rx el Bit 1
		if(Tiempo_Bit > Tiemp_1_Min && Tiempo_Bit < Tiemp_1_Max)
		{
			Bits_IR>>=1;
			Bits_IR|=Uno_Bit_IR;
			cont++;
		}
		if(cont > 15)
			{Banderas.Fin_IR = 1;}
	}
	else
	{
		if(Tiempo_Bit > Tiemp_Start_Min && Tiempo_Bit < Tiemp_Start_Max)
		{
			Banderas.Inicio_IR = 1;
			Banderas.Fin_IR = 0;
			Cont = 0;
			clrear_Buffer();
			T1CON.TMR1ON = 1;
		}
		else
		{

			T1CON.TMR1ON = 1;
			Banderas.Inicio_IR = 0;
			Cont = 0;
			clrear_Buffer();
		}
		
	}

}


//Inicializa Timer 1
void	Inicializa_T1(void)
{
	set_timer1(0);
	T1CON = 0x00;			//Timer 1 como Temporizador, genera un Overflow
							//Cada 65536 uSeg
	PIR1.TMR1IF = 0;
}



void main()
{
	//OSCCON = 0X72;		//Oscilador a 8MHz
	//while(!OSCCON.IOFS){};

	ANSEL = 0x0;
	ext_int_edge(H_TO_L);
	OPTION_REG.nGPPU = 0;
	WPU.WPU0 = 1;
	Cont = 0;
	Banderas = 0;				//Fin_IR y fin de Rx = 0

	output_low(Relay);

	TRIS_Relay = 0;
	Inicializa_T1();
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);
	while(TRUE)
	{
		if(Banderas.Fin_IR)
		{
			disable_interrupts(INT_EXT);
			Banderas.Fin_IR = 0;
			Banderas.Inicio_IR = 0;
			compara=0;
			cont=0;
//
			if(Bits_IR==Tecla_STOP)		//0x4039
			{
				output_low(Relay);
				compara=1;
			}
			else if(Bits_IR==Tecla_PLAY)
			{
				output_high(Relay);
				compara=1;
			}
			else if(Bits_IR==Tecla_FF)
			{
				output_low(Relay);
				delay_ms(3000);
				output_high(Relay);
				compara=1;
			}			

			delay_ms(3000);
			INTCON.INTF = 0;
			enable_interrupts(INT_EXT);

		}
	
	}

}
