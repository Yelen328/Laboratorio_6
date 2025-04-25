/*
 * Prelaboratorio_6.c
 *
 * Created: 25/4/2025 00:12:12
 * Author : yelena Cotzojay
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
//Prototipos de función
void setup();
void initUART();
void WriteChar(char caracter);

int main(void)
{
    setup();
	WriteChar('H');
	WriteChar('O');
	WriteChar('L');
	WriteChar('A');
    while (1) 
    {
    }
}

void setup(){
	cli();
	initUART();
	DDRB =0xFF;		//Configur el puerto B como saldia
	PORTB =0x00;	//Inicialmente apagado
	DDRD |= ((1<<PORTD6)|(1<<PORTD5));	// Configurar el bit 5 y 6 como salida
	PORTD &= ~((1<<PORTD6)|(1<<PORTD5));	//inicialmente apagado
	sei();
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

void WriteChar(char caracter){
	while ( (UCSR0A & (1<<UDRE0))==0){
		
	}

	UDR0=caracter;
	
}

ISR(USART_RX_vect){
	char caracter=UDR0;
	WriteChar(caracter);
	PORTB=caracter;
	
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
