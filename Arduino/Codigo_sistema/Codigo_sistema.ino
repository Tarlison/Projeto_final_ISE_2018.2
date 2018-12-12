#include <ACS712.h>
#define estadoA 0
#define estadoB 1
#define estadoC 2
#define estadoD 3
/*
estadoA = blecaute
estadoB = sem energia da rua e com carga na bateria
estadoC = com energia da rua e com carga parcial na bateria
estadoD = com energia da rua e com carga total na bateria*/
ACS712 sensor(ACS712_30A, A0);

int tensao_bateria = 9; // tensao em Volts;
int capacidade_bateria = 250; //capacidade em mA;
int carga_atual = capacidade_bateria;
int recarregador_bateria = 20; // quanto a placa solar recarrega, em mA;

//Porta ligada ao pino IN1 do modulo
int porta_rele_energiaRua= 2;
//Porta ligada ao pino IN2 do modulo
int porta_rele_placaSolar = 4;
int estadoAtual = estadoA; //estado inicial

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

  switch(estado){
    case estadoA:
      if(I != 0.0){
        Serial.printl("    Recarregando baterias    ");
        //recarregar bateria
        estadoAtual = estadoC;
      }
      break;
    case estadoB:
      if(carga_atual == 0){
        Serial.printl("    OLHA O BLECAUTEEEEE    ");
        estadoAtual = estadoA;
      } else if(I != 0){
        Serial.printl("    Recarregando baterias    ");
        //recarregar bateria
        estadoAtual = estadoC;
      }
      break;
    case estadoC:
      if(I == 0){
        //ativa energia da bateria
        estadoAtual = estadoB;
      } else if(carga_atual == capacidade_bateria){
        //ativa energia da bateria
        estadoAtual = estadoD;
      }
      break;
    case estadoD:
      if(I == 0){
        //ativa energia da bateria
        estadoAtual = estadoB;
      } else if(){
        //recarregar bateria
        estadoAtual = estadoC;
      }
      break;
    default:
      break;
  }
}
