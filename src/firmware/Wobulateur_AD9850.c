/*==============================================================================
par Silicium628
derniere mise à jour 26 octobre 2012
================================================================================
Pilotage d'un circuit DDS AD9850 générateur de fréquence 0..40MHz sinus avec un quartz (pour le AD9850) de 125MHz
-Affichage LCD 2 x 20  caractères ("splitté entre deux ports de l'ATmega)
-Wobulateur (necessite la liaison RS232 avec le soft tournant sur le PC; les commandes sont effectuees depuis le PC)
================================================================================
*/
// #include <math.h>


#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

#include "uart_628.c"



#define	pin_WCL				0b00000100
#define	pin_FQU				0b10000000	// sur port D
#define	pin_RESET			0b01000000
#define	pin_DATA			0b00001000



char * version = "2.1";

/*
RAPPEL variables avr-gcc (vérifiable avec le .map)

char				1   -128 .. 127 ou caractères
unsigned char		1  	0 .. 255 (equiv à byte du pascal)
uint8_t				1		(c'est la même chose que l'affreux 'unsigned char') 
char toto[n]		n
int					2  	-32768 .. 32767	
int16_t				2		idem 'int'
short int			2  	pareil que int (?)
unsigned int		2		0 .. 65535
uint16_t			2		idem 'unsigned int'
long int			4   -2 147 483 648 à 2 147 483 647
int32_t				4		32 bits	;	idem long int
unsigned long long	64-bit unsigned type. 
uint64_t			8		64-bit unsigned type. 
long long int		8	
unsigned long int	4	32 bits	;	0 .. 4 294 967 295  (4,2 x 10^9)
float				4
double				4	// (?)

La déclaration  char JOUR[7][9];
réserve l'espace en mémoire pour 7 mots contenant 9 caractères (dont 8 caractères significatifs).
*/

  uint64_t frequence;
  uint64_t pas_i;
  uint8_t pos;
  uint8_t digits_frq[8];
  uint8_t pas_wob = 2;  // soit 10e2 = 100
  uint64_t	Frq_W32; // valeur binaire destine a l'AD9850, vaut environ 68,72 x frequence en Hz.
  uint8_t phase;
  uint8_t nb_cycles = 0;
	uint8_t octet_recu;
	uint8_t buffer232[20];
	uint8_t compteur1; // pour tests
//	uint8_t flags = 0;



void init_ports (void)  // ports perso
// 0 = entree, 1=sortie   ;   les 1 sur les pins en entrees activent les R de Pull Up (tirage à VCC)
{
  PORTB = 0b00000000;
  DDRB |= 0b00001000;	// portB[3] = sortie (OC2)

  DDRC = 	0b11001111;	//PC4 en entree (acquisition signal) ; PC5 en entree (ADC5 - boutons)

  DDRD =  0b11111111;	//PD1 = TXD ; 
  PORTD = 0b00100000;	//PD0 = RXD 
}



void InitADC (void)
{
  ADCSRA = _BV(ADEN) | _BV(ADPS2);  // Activate ADC with Prescaler 16 --> 1Mhz/16 = 62.5kHz
  ADMUX = 5;    // Select pin ADC5 using MUX
}



void InitINTs (void)
/*
 TCCR2:
wgm21,20 =11 ->Fast PMW
com21,com20=01 ->Set OC2 on Compare Match, clear OC2 at TOP (valable pour le mode Fast PWM);  voir p:116
bits2,1,0:  prescaler (010 = 1/8)
*/
{
  TCCR2=	0b00000000;  // Timer2 non utilisé.
  TIMSK|=	0b00000000;  // INT Timer2 comp disable; INT Timer2 overflow disable;
  GICR|=	0b00000000;  // gere les INTs voir page 67 du pdf
  MCUCR|=	0b00000000;  // The falling edge of INT0 generates an interrupt request. p:67 du pdf
}




void init_variables(void)
{
/*	
1Hz ->		69
10Hz ->		687
100Hz ->	6872
1kHz ->		68719
10kHz ->	687195
100kHz ->	6871948
1MHz ->		68719477
10MHz ->	687 194 767
*/
uint8_t n;
	for (n=0; n<=7; n++)
	{
		digits_frq[n]=0;
	}	
	pos=3;
	digits_frq[pos]=1;
	
  Frq_W32=68719; 
  
  phase = 0;
}



void reset_DDS(void)
{
 _delay_us(1);
  PORTD |=  pin_RESET;
 _delay_us(1);
  PORTD &= ~pin_RESET; // l'operateur "~" donne le complement (inverse des bits). ne pas confondre avec l'opérateur "!"
 _delay_us(1);
}


void impulse_clk_W(void)  // sur pin W_CL
{
 _delay_us(1);
  PORTD |=  pin_WCL;
 _delay_us(1);
  PORTD &= ~pin_WCL; // l'operateur "~" donne le complement (inverse des bits). ne pas confondre avec l'opérateur "!"
 _delay_us(1);
}


void impulse_FQ_U(void)  // sur pin FQ_U
{
 _delay_us(1);
  PORTD |=  pin_FQU;
 _delay_us(1);
  PORTD &= ~pin_FQU; // l'operateur "~" donne le complement (inverse des bits). ne pas confondre avec l'opérateur "!"
 _delay_us(1);
}


void out_DDS(uint64_t freq_i)  // 40 bits vers AD9850; voir son datasheet
{
	uint8_t n;

//envoi de la frequence
  uint32_t masque;

	masque = 1;
  for (n=0;  n<= 31; n++)   // on sort le LSB (W0) en premier
	{
		masque += masque;	// revient à x2 le masque le '1' se deplacant de droite a gauche, 32 bits en tout
		if ( (freq_i & masque) != 0) {PORTD |=  pin_DATA;}	else { PORTD &= ~pin_DATA; }
		
		impulse_clk_W();
	}
	
  PORTD &= ~pin_DATA;				// (W32 toujours = 0)
  impulse_clk_W();

  PORTD &= ~pin_DATA;		// (W33 toujours = 0)
  impulse_clk_W();

  PORTD &= ~pin_DATA;		// (W34 = Power-Down = 0)
  impulse_clk_W();

// envoi de la phase (5 bits)

  for (n=0;  n<=4; n++)    // on sort le LSB (W35) en premier et le MSB en dernier.
	{
		masque = (1 << n);
		if (phase & masque) {PORTD |=  pin_DATA;} else { PORTD &= ~pin_DATA; }

		impulse_clk_W();
	}
// envoi impulsion FQ_UD
	impulse_FQ_U();
}



void calcul_Frq_W32(void)
{
// frequence = digits_frq[0]+digits_frq[1]*10+digits_frq[2]*100+digits_frq[3]*1000+digits_frq[4]*10000+digits_frq[5]*100000+digits_frq[6]*1000000+digits_frq[7]*10000000;  // pour affichage
// J'ai cessé de traiter des grands integers (64bits) pour la représentation de la frequence en decimal, donc commente la ligne precedente (pour gagner place en memoire)

	Frq_W32 = 
	(uint64_t)digits_frq[0]*(uint64_t)69
	+(uint64_t)digits_frq[1]*(uint64_t)687
	+(uint64_t)digits_frq[2]*(uint64_t)6872
	+(uint64_t)digits_frq[3]*(uint64_t)68719
	+(uint64_t)digits_frq[4]*(uint64_t)687195
	+(uint64_t)digits_frq[5]*(uint64_t)6871948
	+(uint64_t)digits_frq[6]*(uint64_t)68719477
	+(uint64_t)digits_frq[7]*(uint64_t)687194767;
// pour DDS
	
/* REMARQUE:
Les (uint64_t) forcent les calculs sur 64 bits et evitent le bug suivant mais consomment 1100 octets en memoire flash	
detail du bug sans les (uint64_t) -> les frequences entre 476 et 954 Hz ne sont pas synthetisees
476*68.72 = 32710	-> ok
477*68.72 = 32779  -> HS 
954*68.72 = 65558  -> reprise mais HS -> qq Hz 
*/	
}



void regle_Frq_00 (void)
{
	// on regle la frequence sur 0 (absence de signal de sortie)
	digits_frq[7]=0; digits_frq[6]=0; digits_frq[5]=0; digits_frq[4]=0; digits_frq[3]=0; digits_frq[2]=0; digits_frq[1]=0; digits_frq[0]=0;
	calcul_Frq_W32();
//	affiche_frequence();
}




void reception_params()
{
		uint8_t n;	
		uint8_t ok = 0;

		compteur1++;

		for (n=0; n<10; n++)
		{
			octet_recu=USART_RxByte();
			buffer232[n]=octet_recu;
		}
		
		
		pas_wob= 10*(buffer232[0]-48) + buffer232[1]-48;

		pas_i=1;
		for (n=1; n<= pas_wob; n++) {pas_i *= 2;}		
		
		for (n=0; n<8; n++)
		{
			digits_frq[n]=buffer232[n+2]-48;
		}	
		calcul_Frq_W32();
		out_DDS(Frq_W32);		
}



void balayage_wob(void)
{
	uint8_t octet_recu;
	uint32_t n;
	uint16_t valeur_lue;
	uint8_t n3H;
	uint8_t n3L;	
	uint8_t compteur2;
	uint64_t freq_i;	
	
	ADMUX = 4;    // Select pin ADC4 using MUX

	// attend feu vert du PC
	octet_recu=USART_RxByte();
	while (octet_recu !='G') {octet_recu=USART_RxByte();}
	{
		
		USART_TxStr("DAT");
		compteur2=0;
		
		freq_i = Frq_W32;
		out_DDS(freq_i);	
		
		for (n=0; n<510; n++)
		{
//acquisition
			_delay_ms(30); // bloqueur (de la valeur analogique mesuree) pendant l'acquisition
			
			ADCSRA |= _BV(ADSC);    //Start conversion - resolution 10bits
			while (ADCSRA & _BV(ADSC) ) {;}    // attend la fin de la converstion
			valeur_lue = ADCW;    // lit la value convertie			
			valeur_lue = 1024 - valeur_lue; // inversion y
			
			// valeur_lue=n; // pour test  (=ok)
			
			n3L=valeur_lue % 128; //codage sur 7 bits
			n3L &= 0b01111111; // 8eme bit de l'octet L toujours  = 0 - (cette ligne est en fait inutile)
			n3H=valeur_lue/128;	//codage sur 7 bits
			// on reserve le 8eme bit pour identifier l'octet (L ou H)
			n3H |= 0b10000000; // 8eme bit de l'octet H toujours  = 1
			
// envoi donnee vers PC				
			USART_TxByte(n3L);
			_delay_us(100);
			USART_TxByte(n3H);
			_delay_us(100);
		
			compteur2++;

			freq_i += pas_i;
			out_DDS(freq_i);
		}
	}

}


void wobulateur (void)
{

	uint8_t compteur;

		
	while(1)
	{	
		octet_recu=USART_RxByte(); 
		
		while ( (octet_recu != 'P') && (octet_recu != 'G'))  { 	octet_recu=USART_RxByte(); }
		
		if (octet_recu == 'P') 	{	reception_params();	}
		else if (octet_recu == 'G')	{	balayage_wob();	}		
		
	}
}



void test (void)
{

}


int main (void)
{
	init_variables(); 
	init_ports();
	InitADC();
	InitINTs();
	USART_Init();
	
	reset_DDS();
		
	
	_delay_ms(2000);
	

	uint8_t compteur;
	compteur=20;
	
	uint32_t f1;
	uint32_t f2;
	uint32_t f3;
	
	uint8_t a;
	uint8_t b;
	uint8_t c;	
	

	Frq_W32 = a*f1+b*f2+c*f3;
	calcul_Frq_W32();

	out_DDS(Frq_W32);
	out_DDS(Frq_W32);
	

	wobulateur();


	
}

