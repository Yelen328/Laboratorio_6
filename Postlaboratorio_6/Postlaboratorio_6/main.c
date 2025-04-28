/*
 * Postlaboratorio_6.c
 *
 * Created: 27/4/2025 17:33:01
 * Author : yelena cotzojay
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>  // Para la funci�n itoa()

//Prototipos de funci�n
void setup();
void initUART();
void INIT_ADC();
void WriteChar(char caracter);
void printMenu();
void writeString (char* texto);
volatile uint8_t  valor_ADC=0;
volatile char caracter;
volatile uint8_t menu_option=0;
volatile uint8_t waiting_for_led_char = 0;  // Nueva bandera para esperar entrada LED
char valorStr[4];  // Para almacenar el valor convertido a string


int main(void)
{
    setup();
	//Iniciar la primera conversi�n
	ADCSRA |= (1 << ADSC);
	
	
    while (1) 
    {
		if (menu_option==0){
			printMenu();
			menu_option=255;	//esperar nueva opci�n
		}
		if (caracter=='1'){
			
			itoa(valor_ADC, valorStr, 10);  // Convertir el valor de ADC a string
			writeString("Valor Potenciometro: ");
			writeString(valorStr);  // Enviar el valor como cadena
			writeString("\n\r");  // A�adir un salto de l�nea para claridad
			caracter = 0; // Resetear la opci�n
			menu_option = 0; // Mostrar men� nuevamente
		}
		
		else if (caracter == '2') {
			writeString("ingrese el car�cter");
			waiting_for_led_char=1; //Activar bandera de espera
			caracter=0;
			while(waiting_for_led_char){}
			menu_option=0;
			}
		}
	
	 }
		


void setup(){
	cli();
	initUART();
	INIT_ADC();
	DDRB =0xFF;		//Configur el puerto B como saldia
	PORTB =0x00;	//Inicialmente apagado
	DDRD |= ((1<<PORTD6)|(1<<PORTD5));	// Configurar el bit 5 y 6 como salida
	PORTD &= ~((1<<PORTD6)|(1<<PORTD5));	//inicialmente apagado
	sei();
}

void printMenu() {
	writeString("\n\rSeleccione una opcion:\n\r");
	writeString("1. Leer potenciometro\n\r");
	writeString("2. Enviar Ascii\n\r");
}

void initUART(){
	//Configurar pines PD0 (TX) y PD1 (TX)
	DDRD |= (1<<DDD1);
	DDRD &= ~(1<<DDD0);
	
	//Configurar UCSR0A
	UCSR0A=0;
	//Configuraci�n UCSR0B: Habililitndo la interrupci�n al escribir:
	//-Habilibitar recepci�n
	//-Habilitaci�n de transmisi�n
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)| (1<< TXEN0);
	//Configurar UCSR0C
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	//Configurar UBRR0: UBRR0 = 103 -> 9600  @ 16MHz
	UBRR0 = 103;
}

void INIT_ADC(){
	ADMUX = 0;	//Apagar todo
	
	//Voltaje de referencia 5V
	ADMUX |=(1<<REFS0);
	
	ADMUX |= (1<<ADLAR); //orientaci�n (izquierda)
	ADMUX |= (1<<MUX2) | (1<<MUX1);//Selecci�n de canal Bit 6 del puerto C
	
	ADCSRA = 0;	//Apagar todo
	ADCSRA |=(1<<ADPS1)	| (1 << ADPS0); //Configuraci�n del presacaler 8
	ADCSRA |= (1 << ADIE);	//Habilitaci�n de interrupciones
	ADCSRA |= (1 << ADEN);	//Habilitar ADS
}


void WriteChar(char caracter){
	while ( (UCSR0A & (1<<UDRE0))==0){
		
	}

	UDR0=caracter;
	
}

//puntero
void writeString(char* texto)
{
	for(uint8_t i = 0; *(texto+i) !='\0'; i++)
	{
		WriteChar(*(texto+i));
	}
	
}

//**************VECTORES DE INTERRUPCI�N*******************/
//Vector de interrupci�n USART
ISR(USART_RX_vect){
	caracter=UDR0;
	
	if (waiting_for_led_char){
		PORTB= caracter;
		writeString("\n\rLEDs actualizados con: ");
		writeString(valorStr);
		writeString("\n\r");
		waiting_for_led_char = 0;  // Desactivar bandera
	}
	
	else{
	WriteChar(caracter);
	writeString("\n\r");  // A�adir un salto de l�nea 
	}
	
	if (caracter & (1 << 6))
	{
		PORTD |= (1 << PORTD6);
		}else{
		PORTD &= ~(1 << PORTD6);
	}
	if (caracter & (1 << 7))
	{
		PORTD |= (1 << PORTD7);
		}else{
		PORTD &= ~(1 << PORTD7);
	}
}

//Vector de interrupci�n ADC
ISR(ADC_vect)
{
	valor_ADC=ADCH;
	// Iniciar nueva conversi�n
	ADCSRA |= (1 << ADSC);
}



