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
      

    #define Luz_01        PIN_A5
    #define Luz_02        PIN_A4

    #define TRIS_Luz01    TRISIO.TRISIO5
	#define TRIS_Luz02    TRISIO.TRISIO4
//
//Para los Switche de las luces tenemos a : A0 (GP0) como SW_Luz_01
//	y a A1 (GP1) como SW_Luz_02
	#define Sw_Luz_01  	PIN_A0
	#define Sw_Luz_02  	PIN_A1
    #define TRIS_Sw_01    TRISIO.TRISIO0
	#define TRIS_Sw_02    TRISIO.TRISIO1
//Para activar las reristencia Pull-UP de A0 y A1se define lo siguiente
	#define PulUp_Luz_01	WPU.WPU0
	#define PulUp_Luz_02	WPU.WPU1

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
			Bits_IR = 0;
			T1CON.TMR1ON = 1;
		}
		else
		{

			T1CON.TMR1ON = 1;
			Banderas.Inicio_IR = 0;
			Cont = 0;
			Bits_IR = 0;
		}
		
	}

}

//Interrupcion por cambio de nivel, Aqui estan los Switche que conmutan el
//	estado de los relay
#INT_RA
Ver_Sw()
{          //Interrupcion por cambio de estado en pines
	delay_ms(100);			//Demora para Anti rrebote

   	if(!input(Sw_Luz_01))		//Ver si El Switche cambia la Luz 01
	{
		if(Banderas.Est_Luz_01)
		{
			output_low(Luz_01);
			Banderas.Est_Luz_01 = 0;
		}
		else
		{
			output_high(Luz_01);
			Banderas.Est_Luz_01 = 1;
		}
   	}
   	if(!input(Sw_Luz_02))		//Ver si El Switche cambia la Luz 02
	{
		if(Banderas.Est_Luz_02)
		{
			output_low(Luz_02);
			Banderas.Est_Luz_02 = 0;
		}
		else
		{
			output_high(Luz_02);
			Banderas.Est_Luz_02 = 1;
		}
   	}   	
} //Fin Int por Cambio

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
	PulUp_Luz_01 = 1;			//Pul-UP Sw Luz 01
	PulUp_Luz_02 = 1;			//Pul-UP Sw Luz 02
	Cont = 0;
	Banderas = 0;				//Fin_IR y fin de Rx = 0
	output_low(Luz_01);
	output_low(Luz_02);

	TRIS_Luz01 = 0;
	TRIS_Luz01 = 0;
	
	Inicializa_T1();
   	enable_interrupts(INT_EXT);
   	enable_interrupts(INT_RA0);
   	enable_interrupts(INT_RA1);
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
			if(Bits_IR==Tecla_O_II)		//0x0817
			{
				if(Banderas.Est_Luz_01)
				{
					output_low(Luz_01);
					Banderas.Est_Luz_01 = 0;
				}
				else
				{
					output_high(Luz_01);
					Banderas.Est_Luz_01 = 1;
				}
			}
			else if(Bits_IR==Tecla_O_ID)
			{
				if(Banderas.Est_Luz_02)
				{
					output_low(Luz_02);
					Banderas.Est_Luz_02 = 0;
				}
				else
				{
					output_high(Luz_02);
					Banderas.Est_Luz_02 = 1;
				}
			}
			delay_ms(1000);
			INTCON.INTF = 0;
			enable_interrupts(INT_EXT);

		}
	
	}

}
