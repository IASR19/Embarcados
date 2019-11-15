#include "notas.h" // Chama a aba de notas p/ o buzzer
#define buzzer 11 // Define o pino onde o buzzer será conectado 
#define compasso 1500 // Altera o compasso da música 
#define Pinofalante buzzer
#define tempo_pin 10

int erros = 0, acertos, i, j;
String segredo = " "; // Altere nesta String a palavra secreta
const String dica = " "; // Altere nesta linha a dica sobre a palavra utilizada
  Serial.println("Entre com o segredo: ");
  segredo = Serial.read();
  Serial.println("Entre com a dica: ");
  dica = Serial.read();
String tentativas = " -", test;
char inChar;
int frequencia = 0;
double sinVal;
int toneVal;

const int gameover[] = {
  NOTE_C5, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_AS4, NOTE_GS4,   NOTE_E4, NOTE_D4, NOTE_E4
};

const int tempo_gameover[] = {
  4, 4, 4, 8, 8, 8, 8, 8, 8,
  12, 12, 4
};

boolean temletra = 0;

void reset() {
  asm volatile ("  jmp 0"); // Função para ativação do Watchdog
}

void setup()
{
  unsigned char temp = Serial.available(); // Limpar a buffer serial ("Serial.flush()" não faz mais isso)
  for (int i = temp; i >= 0; i--)
    Serial.println();
  Serial.read();
  Serial.begin(9600);
  DDRD = 0xFC;
  PORTD = 0xFC;
  delay(1000);
  PORTD = 0x00;
  delay(1000);
  PORTD = 0xFF;
  pinMode(Pinofalante, OUTPUT); //Pino do buzzer

  segredo.toUpperCase();
  Serial.println();
  Serial.println("************************ JOGO DA FORCA ************************");
  Serial.println();
  Serial.print("DICA: ");
  Serial.println(dica);
  Serial.println();

  for (i = 0; i < segredo.length(); i++) {
    if (segredo[i] == 0x20) {
      test += " ";
    }
    else if (segredo[i] == 0x2D) {
      test += "- ";
    }
    else {
      test += "_ ";
    }
  }

  for (i = 0; i < (segredo.length() * 2); i++)
  {
    Serial.print(test[i]);
  }
  Serial.println();
  Serial.println();
}

void loop()
{

  while (Serial.available() > 0) {
    char caracter = Serial.read();

    if (caracter == '0')
      reset();

    if (isAlphaNumeric(caracter) || isDigit(caracter)) {
      caracter = toupper(caracter);
      if (caracter == '0')
        for (j = 0; j < tentativas.length(); j++) {
          if ( tentativas[j] == (caracter) ) {
            temletra = 1;
          }
        }

      if (temletra == 1)
      {
        Serial.println("LETRA JÁ DIGITADA");
        temletra = 0;
      }

      else
      {
        tentativas += caracter;

        if (segredo.indexOf(caracter) < 0 ) {
          erros++;
        }

        acertos = 0;

        for (j = 0; j < tentativas.length(); j++) {
          for (i = 0; i < segredo.length(); i++) {
            if ( segredo[i] == tentativas[j]) {
              test[i * 2] =  segredo[i];
              acertos++;
            }
          }
        }

        Serial.print("-> ERROS: ");
        Serial.println(erros);
        Serial.println();

        //-----------------------------------------------------------------------

        // imprime letras na tela
        for ( i = 0; i < (segredo.length() * 2); i++) {
          Serial.print(test[i]);
        }
        Serial.println();
        Serial.println();
      }
    }

    else {
      Serial.println("* DIGITE A LETRA");
      Serial.println();
    }

    //Atualiza LEDs que indicam os erros
    PORTD = 0xFF << (erros + 2);

    if (acertos == segredo.length()) {
      Serial.println("                 VOCÊ VENCEU!!!");
      for (int x = 0; x < 2; x++) {
        for (frequencia = 150; frequencia < 1800; frequencia += 1)
        {
          tone(Pinofalante, frequencia, tempo_pin);
          delay(1);
        }
        for (frequencia = 1800; frequencia > 150; frequencia -= 1)
        {
          tone(Pinofalante, frequencia, tempo_pin);
          delay(1);
        }
      }
      Serial.println("                 DIGITE '0' P/ REINICIALIZAR"); // Reinício
      boolean para = true;
      while (para) {
        char c1 = Serial.read();
        if (c1 == '0')
        {
          reset();
          para = false;
        }

      }
    }

    if (erros >= 6) {
      Serial.println("                 GAME OVER");       // Toca musica de GAME OVER
      for (int Nota = 0; Nota < 12; Nota++) {
        int tempo = compasso / tempo_gameover[Nota];
        tone(11, gameover[Nota], tempo);
        delay(tempo * 1.2);
      }
      Serial.println("                 DIGITE '0' P/ REINICIALIZAR"); // Reinício
      boolean para = true;
      while (para) {
        char c1 = Serial.read();
        if (c1 == '0')
        {
          reset();
          para = false;
        }

      }
    }
  }
}
