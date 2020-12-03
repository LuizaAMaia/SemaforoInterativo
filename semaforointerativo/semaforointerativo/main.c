#define F_CPU 16000000UL//frequencia da cpu 16mhz
#include <avr/io.h>//incluindo entrada e saída
#include <util/delay.h>//incluindo delays
#include <avr/interrupt.h>//incluindo interrupções
#include "nokia5110.h"//incluindo funções do display nokia
#define BAUD 9600 //taxa de comunicaçao
#define MYUBRR F_CPU/16/BAUD-1//
int VALORES[3] = {0x10,0x20,0x40};//vetor com valores para acender os leds
int mk;//variável de controle do tempo de ativação dos sinais
int f=0,p=0;//inicializa variáveis de controle com zero

ISR (INT0_vect){
	f=3;//interrupção que habilita f=3
}

ISR (INT1_vect){
	p = p+1;//interrupção que incrementa p

	if(p%2==1){
		nokia_lcd_init();//inicia a tela
		nokia_lcd_clear();//limpa a tela
		nokia_lcd_write_string("operacao normal",1);//escreve mensagem na tela
		nokia_lcd_set_cursor(0,10);//posiciona o cursor na tela
		nokia_lcd_render();//atualiza tela
		f=1;
	}
	
	if(p%2==0){
		PORTC = VALORES[1];//acende led amarelo
		nokia_lcd_init();
		nokia_lcd_clear();
		nokia_lcd_write_string("desligado/manutencao",1);//mensagem de manutençao
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_render();
		f=4;
	}
	
}


ISR(ADC_vect){
	
}


void USART_Init(unsigned int ubrr){
	
	UBRR0H = (unsigned char)(ubrr>>8);//ajusta transmissão
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);//habilita transmissor e receptor
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);//ajusta formato do frame
	DDRC = 0xFF;
	
}
//funçao para envio de frame
void USART_Transmit(unsigned char data){

	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}
//funçao para recepção de frame
unsigned char (USART_Receive(void)){
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}


ISR(USART_RX_vect){

	char recebido;//variavel do tipo char
	recebido = UDR0;//a variavel do tipo char recebe argumento do UDR0
	
	if(recebido == 'a')//se usart recebe 'a', modo de operação 1
	f = 1;
	if(recebido == 'b')//se usart recebe 'b', modo de operação 2
	f = 2;
	if(recebido == 'p')//se usart recebe p, porta fica em nível alto e ativa botão panico
	PORTB = PORTB^0x01;
	if(recebido == 'u')//se usart recebe u, porta fica em nível baixo e desativa botão panico
	PORTB = PORTB^0x01;
}



int main(void)
{
	USART_Init(MYUBRR);
	DDRD = 0b01000000;//define todos os pinos D como entrada, menos d6
	PORTD = 0b10111111;//pull-up ativado em todos menos no pino 6
	DDRB = 0xFF; //todas os pinos b como saída
	DDRC = 0b11110000; //pinos superiores c como saída e inferiores como entrada
	PORTC = 0b00001111; //pull-up ativado  para inferiores c
	ADMUX = 0b01000000;// referencia para adc
	ADCSRA = 0b11101111;//habilitando AD,conversão, prescaler e interrupção
	ADCSRB = 0x00;// para habilitar conversão continua
	DIDR0 = 0b00111110;//habilitando c0 para entrada do adc
	TCCR0A = 0b10100011;//ajusta pwm não invertido
	TCCR0B = 0b00000101;//liga pino e ajuste do prescaler do pwm
	EICRA = 0b00001010;//habilita interrupção externa int0 e int1 na borda de descida
	EIMSK = 0b00000011;//habilita int0 e int1
	sei();//habilita interrupções globais
	nokia_lcd_init();//inicia a tela
	
	
	while (1)
	{
		
		OCR0A = (ADC/4)-1; //comparador vai receber valor do adc - 1
		
		switch (f)
		{
			case 1:
			PORTC = VALORES[2]; //acender led verde
			for(mk=0;mk<=5;mk++)//cada for desse determina o tempo que o led fica aceso
			_delay_ms(1000); //delay 1 de s
			PORTC = VALORES[1]; //acender led amarelo
			_delay_ms(1000);//delay 1 de s
			PORTC = VALORES[0]; //acender led vermelho
			for(mk=0;mk<=2;mk++)//cada for desse determina o tempo que o led fica aceso
			_delay_ms(1000);//delay 1 de s
			break;//parando esse caso
			
			case 2:
			PORTC = VALORES[2];//acender led verde
			for(mk=0;mk<=2;mk++)//cada for desse determina o tempo que o led fica aceso
			_delay_ms(1000);//delay 1 de s
			PORTC = VALORES[0];//acender led vermelho
			for(mk=0;mk<=5;mk++)//cada for desse determina o tempo que o led fica aceso
			_delay_ms(1000);//delay 1 de s
			break;//parando esse caso
			
			case 3:
			nokia_lcd_init();//inicia a tela
			nokia_lcd_clear();//limpa a tela
			nokia_lcd_write_string("pedestre na faixa",1);//escreve mensagem na tela
			nokia_lcd_set_cursor(0,10);//posiciona o cursor
			nokia_lcd_render();//atualiza a tela
			PORTC = VALORES[0];//acender led vermelho
			for(mk=0;mk<=8;mk++)//cada for desse determina o tempo que o led fica aceso
			_delay_ms(1000);//delay 1 de s
			nokia_lcd_clear();//limpa a tela
			nokia_lcd_render();//atualiza a tela
			f=1;//retorna para o modo 1
			nokia_lcd_init();//inicia a tela
			nokia_lcd_clear();//limpa a tela
			nokia_lcd_write_string("operacao normal",1);//escreve string na tela
			nokia_lcd_set_cursor(0,10);//posiciona cursor
			nokia_lcd_render();//atualiza tela
			break;//parando esse caso
			
			
			case 4:
			PORTC = VALORES[1];//acender led amarelo
			break;//parando esse caso
			
			
			
		}
	}
}