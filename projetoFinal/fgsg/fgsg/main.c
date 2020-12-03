#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nokia5110.h"
#define tam_vetor 3
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
int VALORES[3] = {0x10,0x20,0x40};
int nk=0;mk;
int f=0,p=0;

ISR (INT0_vect){
	f=3;
}

ISR (INT1_vect){
	p = p+1;
	if(p%2==1){
		nokia_lcd_init();
		nokia_lcd_clear();
		nokia_lcd_write_string("operacao normal",1);
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_render();
		f=1;
	}
	
	if(p%2==0){
		PORTC = VALORES[1];
		nokia_lcd_init();
		nokia_lcd_clear();
		nokia_lcd_write_string("desligado/manutencao",1);
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_render();
		f=4;
	}
	
}


ISR(ADC_vect){
	
}


void USART_Init(unsigned int ubrr){
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	DDRC = 0xFF;
	
}
void USART_Transmit(unsigned char data){

	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

unsigned char (USART_Receive(void)){
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}


ISR(USART_RX_vect){

	char recebido;
	recebido = UDR0;
	
	if(recebido == 'a')
	f = 1;
	if(recebido == 'b')
	f = 2;
	if(recebido == 'p')
	PORTB = PORTB^0x01;
	if(recebido == 'u')
	PORTB = PORTB^0x01;
}



int main(void)
{
	USART_Init(MYUBRR);
	DDRD = 0b01000000;
	PORTD = 0b10111111;
	DDRB = 0xFF;
	DDRC = 0b11110000;
	PORTC = 0b00001111;
	ADMUX = 0b01000000;
	ADCSRA = 0b11101111;
	ADCSRB = 0x00;
	DIDR0 = 0b00111110;
	TCCR0A = 0b10100011;
	TCCR0B = 0b00000101;
	EICRA = 0b00001010;
	EIMSK = 0b00000011;
	sei();
	nokia_lcd_init();
	
	
	while (1)
	{
		
		OCR0A = (ADC/4)-1;
		
			switch (f)
			{
				case 1:
				PORTC = VALORES[2];
				for(mk=0;mk<=5;mk++)
				_delay_ms(1000);
				PORTC = VALORES[1];
				_delay_ms(1000);
				PORTC = VALORES[0];
				for(mk=0;mk<=2;mk++)
				_delay_ms(1000);
				break;
				
				case 2:
				PORTC = VALORES[2];
				for(mk=0;mk<=2;mk++)
				_delay_ms(1000);
				PORTC = VALORES[0];
				for(mk=0;mk<=5;mk++)
				_delay_ms(1000);
				break;
				
				case 3:
				nokia_lcd_init();
				nokia_lcd_clear();
				nokia_lcd_write_string("pedestre na faixa",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_render();
				PORTC = VALORES[0];
				for(mk=0;mk<=8;mk++)
				_delay_ms(1000);
				nokia_lcd_clear();
				nokia_lcd_render();
				f=1;
				nokia_lcd_init();
				nokia_lcd_clear();
				nokia_lcd_write_string("operacao normal",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_render();
				break;
				
				
				case 4:
				PORTC = VALORES[1];
				break;
			
			
			
		}
	}
}