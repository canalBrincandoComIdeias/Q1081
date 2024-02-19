#include <LiquidCrystal_I2C.h>

#define pinSW 12
#define pinDT 2   //Sentido anti-horário
#define pinCLK 3  //Sentido horário

int estadoRotary = 0; //0=Parado 1=CLK 2=DT 3=CLK_Centro 4=DT_Centro
void processaCLK();
void processaDT();

/*Diagrama da lógica do rotary
  0 ___ 1 ___ 3 ___ ConfirmaCLK
     |     |_ 3 ___ Cancela
     |     |_ Cancela
     |_ 2 ___ 4 ___ ConfirmaDT
           |_ 4 ___ Cancela
           |_ Cancela
*/

int ponteiro = 0;

boolean atualiza = true;

String  menu[]  = {"Lampada", "Bomba 1", "Bomba 2", "Exalstor"};  //Tamanho máximo do texto: 11 caracteres
boolean valor[] = {LOW, LOW, LOW, LOW};

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(pinSW, INPUT);
  pinMode(pinDT, INPUT);
  pinMode(pinCLK, INPUT);

  lcd.init();
  lcd.backlight();

  attachInterrupt(digitalPinToInterrupt(pinCLK), rotary_loop, CHANGE);   //No Arduino UNO só funciona na porta 2 ou 3
  attachInterrupt(digitalPinToInterrupt(pinDT), rotary_loop, CHANGE);    //No Arduino UNO só funciona na porta 2 ou 3
}

void loop() {
  //Trata Botão do Rotary
  if (!digitalRead(pinSW)) {
    valor[ponteiro] = !valor[ponteiro];
    atualiza = true;
    while (!digitalRead(pinSW));
  }


  //Atualiza Display
  if (atualiza) {
    atualiza = false;

    lcd.setCursor(0, 0); //coluna, linha
    lcd.print(">");
    lcd.print(menu[ponteiro]);
    lcd.print("                ");

    lcd.setCursor(13, 0); //coluna, linha
    if (valor[ponteiro]) {
      lcd.print("ON");
    } else {
      lcd.print("OFF");
    }

    lcd.setCursor(0, 1); //coluna, linha
    if (ponteiro == (sizeof(valor) - 1)) { //Se chegou no final do menu
      lcd.print("                ");
    } else {
      lcd.print(" ");
      lcd.print(menu[ponteiro + 1]);
      lcd.print("                ");

      lcd.setCursor(13, 1); //coluna, linha
      if (valor[ponteiro + 1]) {
        lcd.print("ON");
      } else {
        lcd.print("OFF");
      }
    }
  }
}

void rotary_loop() {

  //Lógica para estado PARADO
  if (estadoRotary == 0) {
    if (!digitalRead(pinCLK)) { //Ao girar, acionando primeiro o CLK
      estadoRotary = 1;
    }

    if (!digitalRead(pinDT)) { //Ao girar, acionando primeiro o DT
      estadoRotary = 2;
    }
  }

  //Lógica para estado CLK
  if (estadoRotary == 1) {
    if (!digitalRead(pinDT)) { //Seguir girando, acionando centro
      estadoRotary = 3;
    }

    if (digitalRead(pinCLK)) { //Não completou o giro. CANCELA
      estadoRotary = 0;
    }
  }

  //Lógica para estado DT
  if (estadoRotary == 2) {
    if (!digitalRead(pinCLK)) { //Seguir girando, acionando centro
      estadoRotary = 4;
    }

    if (digitalRead(pinDT)) { //Não completou o giro. CANCELA
      estadoRotary = 0;
    }
  }

  //Lógica para estado CENTRO_CLK
  if (estadoRotary == 3) {
    if (digitalRead(pinCLK)) { //Seguir girando, acionando a confirmacao
      processaCLK();
      estadoRotary = 0;
    }

    if (digitalRead(pinDT)) { //Não completou o giro. CANCELA
      estadoRotary = 0;
    }
  }

  //Lógica para estado CENTRO_DT
  if (estadoRotary == 4) {
    if (digitalRead(pinDT)) { //Seguir girando, acionando a confirmacao
      processaDT();
      estadoRotary = 0;
    }

    if (digitalRead(pinCLK)) { //Não completou o giro. CANCELA
      estadoRotary = 0;
    }
  }
}

//Executa quando o CLK for confirmado
void processaCLK() {

  ponteiro++;
  ponteiro = constrain(ponteiro, 0, sizeof(valor) - 1);
  atualiza = true;
}

//Executa quando o DT for confirmado
void processaDT() {

  ponteiro--;
  ponteiro = constrain(ponteiro, 0, sizeof(valor) - 1);
  atualiza = true;
}
