#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>         // para uso de uint8_t
#include <LiquidCrystal.h>  //Biblioteca para uso do DisplayLCD

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  //Configura pinos do LCD

#define BIT0 0b00000001
#define BIT1 0b00000010
#define BIT2 0b00000100
#define BIT3 0b00001000
#define BIT4 0b00010000
#define BIT5 0b00100000
#define BIT6 0b01000000
#define BIT7 0b10000000





void ADC_init(void) {
  // Configurando Vref para VCC = 5V
  ADMUX = (1 << REFS0);
  /*
    ADC ativado e preescaler de 128
    16MHz / 128 = 125kHz
    ADEN = ADC Enable, ativa o ADC
    ADPSx = ADC Prescaler Select Bits
    1 1 1 = clock / 128
  */
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int ADC_read(u8 ch) {
  char i;
  int ADC_temp = 0;  // ADC temporário, para manipular leitura
  int ADC_read = 0;  // ADC_read
  ch &= 0x07;
  // Zerar os 3 primeiros bits e manter o resto
  ADMUX = (ADMUX & 0xF8) | ch;
  // ADSC (ADC Start Conversion)
  ADCSRA |= (1 << ADSC);  // Faça uma conversão
  // ADIF (ADC Interrupt Flag) é setada quando o ADC pede interrupção
  // e resetada quando o vetor de interrupção
  // é tratado.
  while (!(ADCSRA & (1 << ADIF)))
    ;                      // Aguarde a conversão do sinal
  for (i = 0; i < 8; i++)  // Fazendo a conversão 8 vezes para maior precisão
  {
    ADCSRA |= (1 << ADSC);  // Faça uma conversão
    while (!(ADCSRA & (1 << ADIF)))
      ;                       // Aguarde a conversão do sinal
    ADC_temp = ADCL;          // lê o registro ADCL
    ADC_temp += (ADCH << 8);  // lê o registro ADCH
    ADC_read += ADC_temp;     // Acumula o resultado (8 amostras) para média
  }
  ADC_read = ADC_read >> 3;  // média das 8 amostras ( >> 3 é o mesmo que /8)
  return ADC_read;
}


int main() {

  u16 leitura;
  float tensao;
  Serial.begin(9600);
  ADC_init();        // Inicializa ADC
  lcd.begin(16, 2);  //Inicializa Display
  lcd.print("Projeto - E209");
  lcd.setCursor(0, 1);
  lcd.print("Itamar-Ana");
  _delay_ms(300);

  DDRB |= (BIT0 + BIT1 + BIT2);    //PB0, PB1, PB2 e PB3 definidos como saida
  PORTB |= (BIT5);                 //PB5 ativado PULL UP
  PORTB &= ~(BIT0 + BIT1 + BIT2);  //desativa PB0, PB1, PB2 e PB3



  for (;;) {
    leitura = ADC_read(ADC4D);  // lê o valor do ADC0 = PC0
    _delay_ms(50);              // Tempo para troca de canal

    tensao = (leitura * 5.0) / 1023;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nivel de grau");
    lcd.setCursor(0, 1);
    lcd.print(tensao);
    Serial.println(tensao);
    Serial.println(leitura);
    _delay_ms(150);
    int liga = !(PINB & BIT5);
    if (liga == 0) {
      if (leitura > 600) {
        lcd.setCursor(5, 1);
        lcd.print(">> NO GRAU");
        PORTB |= (BIT0);
        PORTB &= ~(BIT1 + BIT2);
      }

      else if (leitura >= 400 && leitura < 600) {
        lcd.setCursor(5, 1);
        lcd.print(">> GRAU +/-");
        PORTB |= (BIT1);
        PORTB &= ~(BIT0 + BIT2);
      }

      else {
        lcd.setCursor(5, 1);
        lcd.print(">> SEM GRAU");
        PORTB |= (BIT2);
        PORTB &= ~(BIT0 + BIT1);
      }
    }
  }
}