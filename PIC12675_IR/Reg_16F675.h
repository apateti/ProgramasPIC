#nolist
    struct INTCONmap 
	{ 
	boolean RAIBIF; 
	boolean INTF; 
	boolean TMR0IF; 
	boolean RAIBIE; 
	boolean INTE; 
	boolean TMR0IE; 
	boolean PEIE; 
	boolean GIE; 
	} INTCON; 
	#byte INTCON=0x0B

//Estructura del Reg OPTION_REG
    struct OPTION_REGmap 
	{
	boolean PS0;
	boolean PS1;
	boolean PS2;
	boolean PSA;
	boolean T0SE;
	boolean T0CS;
	boolean INTEDG; 
	boolean nGPPU; 
	} OPTION_REG; 
	#byte OPTION_REG=0x81

 //Estructura del Reg PIR1   
    struct PIR1map 
	{ 
	boolean TMR1IF; 
	boolean TMR2IF; 
	boolean CCP1IF; 
	boolean SSPIF; 
	boolean TXIF; 
	boolean RCIF; 
	boolean ADIF; 
	boolean NOT; 
	} PIR1; 
	#byte PIR1=0x0C
	
 //Estructura del Reg PIE1  
    struct PIE1map 
	{ 
	boolean TMR1IE; 
	boolean TMR2IE; 
	boolean CCP1IE; 
	boolean SSPIE; 
	boolean TXIE; 
	boolean RCIE; 
	boolean ADIE; 
	boolean NOT_PIE; 
	} PIE1; 
	#byte PIE1=0x8C	

//Estructura del Reg T1CON   
    struct T1CONmap 
	{ 
	boolean TMR1ON; 
	boolean TMR1CS; 
	boolean T1SYNC; 
	boolean T1OSCEN; 
	boolean T1CKPS0; 
	boolean T1CKPS1; 
	boolean TMR1GE; 
	boolean T1GINV; 
	} T1CON; 
	#byte T1CON=0x10

//Estructura del Reg TRISIO  
    struct TRISIOmap 
	{ 
	boolean TRISIO0; 
	boolean TRISIO1; 
	boolean TRISIO2; 
	boolean TRISIO3; 
	boolean TRISIO4; 
	boolean TRISIO5; 
	boolean TRISIO6; 
	boolean TRISIO7; 
	} TRISIO; 
	#byte TRISIO=0x85
	
/*//Estructura del Reg GPIO  
    struct GPIOOmap 
	{ 
	boolean GPIO0; 
	boolean GPIO1; 
	boolean GPIO2; 
	boolean GPIO3; 
	boolean GPIO4; 
	boolean GPIO5; 
	boolean GPIO6; 
	boolean GPIO7; 
	} GPIO; 
	#byte GPIO=0x05*/
	
//Estructura del Reg WPU  
    struct WPUmap 
	{ 
	boolean WPU0; 
	boolean WPU1; 
	boolean WPU2; 
	boolean WPU3; 
	boolean WPU4; 
	boolean WPU5; 
	boolean WPU6; 
	boolean WPU7; 
	} WPU; 
	#byte WPU=0x95	
	
    struct Banderasmap 
	{
	boolean Inicio_IR;
	boolean Fin_IR;
	boolean B_2;
	boolean B_3;
	boolean B_4;
	boolean B_5;
	boolean Est_Luz_01; 
	boolean Est_Luz_02; 
	} Banderas; 
	//#byte Banderas

	#byte ANSEL=0x9F

	
