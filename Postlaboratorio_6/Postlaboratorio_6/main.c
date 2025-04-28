/*
 * Postlaboratorio_6.c
 *
 * Created: 27/4/2025 17:33:01
 * Author : yelena cotzojay
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>  // Para la función itoa()

//Prototipos de función
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
	//Iniciar la primera conversión
	ADCSRA |= (1 << ADSC);
	
	
    while (1) 
    {
		if (menu_option==0){
			printMenu();
			menu_option=255;	//esperar nueva opción
		}
		if (caracter=='1'){
			
			itoa(valor_ADC, valorStr, 10);  // Convertir el valor de ADC a string
			writeString("Valor Potenciometro: ");
			writeString(valorStr);  // Enviar el valor como cadena
			writeString("\n\r");  // Añadir un salto de línea para claridad
			caracter = 0; // Resetear la opción
			menu_option = 0; // Mostrar menú nuevamente
		}
		
		else if (caracter == '2') {
			writeString("ingrese el carácter");
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
	//Configuración UCSR0B: Habililitndo la interrupción al escribir:
	//-Habilibitar recepción
	//-Habilitación de transmisión
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
	
	ADMUX |= (1<<ADLAR); //orientación (izquierda)
	ADMUX |= (1<<MUX2) | (1<<MUX1);//Selección de canal Bit 6 del puerto C
	
	ADCSRA = 0;	//Apagar todo
	ADCSRA |=(1<<ADPS1)	| (1 << ADPS0); //Configuración del presacaler 8
	ADCSRA |= (1 << ADIE);	//Habilitación de interrupciones
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

//**************VECTORES DE INTERRUPCIÓN*******************/
//Vector de interrupción USART
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
	writeString("\n\r");  // Añadir un salto de línea 
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

//Vector de interrupción ADC
ISR(ADC_vect)
{
	valor_ADC=ADCH;
	// Iniciar nueva conversión
	ADCSRA |= (1 << ADSC);
}



