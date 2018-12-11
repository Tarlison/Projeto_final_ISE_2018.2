#include <ACS712.h>

ACS712 sensor(ACS712_30A, A0);

int tensao_bateria = 9; // tensao em Volts;
int capacidade_bateria = 250; //capacidade em mA;
int recarregador_bateria = 20; // quanto a placa solar recarrega, em mA;

//Porta ligada ao pino IN1 do modulo
int porta_rele_energiaRua= 2;
//Porta ligada ao pino IN2 do modulo
int porta_rele_placaSolar = 4;

void setup() {
  Serial.begin(9600);
  sensor.calibrate();
  //Define pinos para o rele como saida
  pinMode(porta_rele_energiaRua, OUTPUT); 
  pinMode(porta_rele_placaSolar, OUTPUT);
}

void loop() {
  float I = sensor.getCurrentAC(60);
  digitalWrite(porta_rele_energiaRua, HIGH);  //Desliga rele 1
  digitalWrite(porta_rele_placaSolar, LOW); //Liga rele 2
  
  
  while (I <= 0.08){
    I = sensor.getCurrentAC(60);
    digitalWrite(porta_rele_energiaRua, LOW);  //Desliga rele 1
    digitalWrite(porta_rele_placaSolar, HIGH); //Liga rele 2
  }
  
  Serial.println(I,5);
  Serial.println("=============");
  delay(1000);
}
