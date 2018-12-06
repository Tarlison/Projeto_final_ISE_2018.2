/*-importacao Necessaria*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
int pinoSensor =A0;
 
int sensorValue_aux = 0;
float valorSensor = 0;
float valorCorrente = 0;
float voltsporUnidade = 0.004887586;// 5%1023
// Para ACS712 de  5 Amperes use 0.185
// Para ACS712 de 10 Amperes use 0.100
//  Para ACS712 de 5 Amperes use 0.066
float sensibilidade = 0.066;
 
//Tensao da rede AC 110 Volts e na verdade (127 volts)
int tensao = 127;
 
 
/*Declaracao de Constates e Objetos*/
// seta o Endereco do Display LCD  0x27
// Seta os pinos do I2C usado (padrao da biblioteca)
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
 
void setup() {
 
 
  //Incia a Serial
  Serial.begin(9600);
  pinMode(pinoSensor, INPUT);
 
  // inicializa o lcd de 20 caracters e 4 linhas
  lcd.begin(20,4);  
  //lcd.begin(16,2);
 
  // ------- Um breve blink de backlight (luz de fundo  -------------
  //liga
  lcd.backlight();
  delay(250);
  //desliga
  lcd.noBacklight();
  delay(250);
 
  //finaliza com a luz de fundo ligada
  lcd.backlight();
 
}
 
void loop() {
 
  for(int i=10000; i>0; i--){
    // le o sensor na pino analogico A0 e ajusta o valor lido ja que a saída do sensor é (1023)vcc/2 para corrente =0
    sensorValue_aux = (analogRead(pinoSensor) -510);
    // somam os quadrados das leituras.
    valorSensor += pow(sensorValue_aux,2);
    delay(1);
  }
 
  // finaliza o calculo da média quadratica e ajusta o valor lido para volts
  valorSensor = (sqrt(valorSensor/ 10000)) * voltsporUnidade;
  // calcula a corrente considerando a sensibilidade do sernsor (185 mV por amper)
  valorCorrente = (valorSensor/sensibilidade);
 
  //tratamento para possivel ruido
  //O ACS712 para 30 Amperes é projetado para fazer leitura
  // de valores alto acima de 0.25 Amperes até 30.
  // por isso é normal ocorrer ruidos de até 0.20A
  //por isso deve ser tratado
  if(valorCorrente <= 0.095){
    valorCorrente = 0;
  }
 
  valorSensor =0;
 
 
  lcd.clear();
  //Reposiciona o cursor na linha 0 e coluna 0
  lcd.setCursor(0,0);
  //Escreve
 
  //Mostra o valor da corrente
  Serial.print("Corrente : ");
  // Irms
  Serial.print(valorCorrente, 3);
  Serial.print(" A ");
  lcd.print("Corrente :");
  lcd.print(valorCorrente, 3);
  lcd.print("A");
 
  //Pula para segunda linha na posicao 0
  lcd.setCursor(0,2);
  //Escreve
  //Calcula e mostra o valor da potencia
  Serial.print(" Potencia (Consumo) : ");
  Serial.print(valorCorrente * tensao);
  Serial.println(" Watts ");
  lcd.print("Potencia (Consumo) : ");
  lcd.setCursor(0,3);
  lcd.print(valorCorrente * tensao);
  lcd.print(" Watts ");
 
 
  delay(100);
 
}
