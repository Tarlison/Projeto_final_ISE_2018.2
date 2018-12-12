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
int capacidade_bateria = 250; //capacidade em mAh;
int carga_atual = 0; // carga em porcentagem;
int recarregador_bateria = 20; // quanto a placa solar recarrega, em mA;
int taxa_consumo = 50; //taxa em mA

//Porta ligada ao pino IN1 do modulo
int porta_rele_energiaRua = 2;
//Porta ligada ao pino IN2 do modulo
int porta_rele_placaSolar = 4;
int estadoAtual = estadoA; //estado inicial

float I = 0;

void imprime_dados(){
  Serial.print("Corrente da rua: ");
  Serial.print(I, 4);
  Serial.println(" A");

  Serial.print("Carga Atual: ");
  Serial.print( (((float)carga_atual/capacidade_bateria)*100), 3);
  Serial.println(" %");
}

void tempo_recarga(){
  Serial.print("Tempo estimado de recarga: ");
  Serial.print((float)(capacidade_bateria - carga_atual)/recarregador_bateria, 3);
  Serial.println("horas");
  carga_atual = carga_atual + recarregador_bateria;
}

void tempo_uso(){
  Serial.print("Tempo estimado de uso: ");
  Serial.print((float)(capacidade_bateria - carga_atual)/taxa_consumo, 3);
  Serial.println("horas");
  carga_atual = carga_atual - taxa_consumo;
}

void setup() {
  Serial.begin(9600);
  sensor.calibrate();
  pinMode(9, OUTPUT);
  
  //Define pinos para o rele como saida
  pinMode(porta_rele_energiaRua, OUTPUT); 
  pinMode(porta_rele_placaSolar, OUTPUT);
  carga_atual = capacidade_bateria;
}

void loop() {
  digitalWrite(9, HIGH);
  I = sensor.getCurrentAC(60);
  digitalWrite(porta_rele_energiaRua, LOW);  //Liga rele 1
  digitalWrite(porta_rele_placaSolar, LOW); //Desliga rele 2
  if(I <= 0.08){
    I = 0;
  }
  if(carga_atual < 0){
    carga_atual = 0;
  }
  imprime_dados();

  switch(estadoAtual){
    case estadoA:
      if(I != 0){
        Serial.println("    Recarregando baterias    "); //recarregar bateria
        tempo_recarga();
        digitalWrite(porta_rele_energiaRua, HIGH);
        digitalWrite(porta_rele_placaSolar, LOW);
        estadoAtual = estadoC;
      }
      break;
    case estadoB:
      if(carga_atual == 0){
        Serial.println("    OLHA O BLECAUTEEEEE    ");
        digitalWrite(porta_rele_energiaRua, LOW);
        digitalWrite(porta_rele_placaSolar, LOW);
        estadoAtual = estadoA;
      } else if(I != 0){
        Serial.println("    Recarregando baterias    ");//recarregar bateria
        digitalWrite(porta_rele_energiaRua, LOW);
        digitalWrite(porta_rele_placaSolar, LOW);
        tempo_recarga();
        estadoAtual = estadoC;
      }
      break;
    case estadoC:
      if(I == 0){
        Serial.println("    Usando energia das baterias    ");//ativa energia da bateria
        digitalWrite(porta_rele_energiaRua, HIGH);
        digitalWrite(porta_rele_placaSolar, HIGH);
        tempo_uso();
        estadoAtual = estadoB;
      } else if(carga_atual == capacidade_bateria){
        Serial.println("    Usando energia das baterias    ");//ativa energia da bateria
        digitalWrite(porta_rele_energiaRua, HIGH);
        digitalWrite(porta_rele_placaSolar, HIGH);
        tempo_uso();
        estadoAtual = estadoD;
      }
      break;
    case estadoD:
      if(I == 0){
        Serial.println("    Usando energia das baterias    ");//ativa energia da bateria
        digitalWrite(porta_rele_energiaRua, HIGH);
        digitalWrite(porta_rele_placaSolar, HIGH);
        tempo_uso();
        estadoAtual = estadoB;
      } 
      else if(carga_atual/capacidade_bateria < 0.5){
        Serial.println("    Recarregando baterias    ");//recarregar bateria
        digitalWrite(porta_rele_energiaRua, LOW);
        digitalWrite(porta_rele_placaSolar, LOW);
        tempo_recarga();
        estadoAtual = estadoC;
      }
      break;
    default:
      break;
  }
  Serial.println("==================");
  delay(3000);
}
