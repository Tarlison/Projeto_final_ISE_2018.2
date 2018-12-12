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
int estadoAtual = estadoD; //estado inicial

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
  Serial.print((float)(capacidade_bateria - carga_atual)/recarregador_bateria, 2);
  Serial.println(" horas");
  carga_atual = carga_atual + recarregador_bateria;
  if(carga_atual > capacidade_bateria){
    carga_atual = capacidade_bateria;
  }
}

void tempo_uso(){
  Serial.print("Tempo estimado de uso: ");
  Serial.print((float)carga_atual/taxa_consumo, 3);
  Serial.println(" horas");
  carga_atual = carga_atual - taxa_consumo;
  if(carga_atual < 0){
    carga_atual = 0;
  }
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
  if(I <= 0.08){
    I = 0;
  }
  imprime_dados();

  switch(estadoAtual){
    case estadoA:
      Serial.println("    OLHA O BLECAUTEEEEE    ");
      digitalWrite(porta_rele_energiaRua, LOW);
      digitalWrite(porta_rele_placaSolar, LOW);
      if(I != 0){
        estadoAtual = estadoC;
      }
      break;
    case estadoB:
      Serial.println("    EB: Usando energia das baterias    ");//ativa energia da bateria
      digitalWrite(porta_rele_energiaRua, HIGH);
      digitalWrite(porta_rele_placaSolar, HIGH);
      tempo_uso();
      if(carga_atual == 0){
        estadoAtual = estadoA;
      } else if(I != 0){
        estadoAtual = estadoC;
      }
      break;
    case estadoC:
      Serial.println("    EC: Recarregando baterias    ");//recarregar bateria
      digitalWrite(porta_rele_energiaRua, LOW);
      digitalWrite(porta_rele_placaSolar, LOW);
      tempo_recarga();
      if(I == 0){
        estadoAtual = estadoB;
      } else if(carga_atual == capacidade_bateria){
        estadoAtual = estadoD;
      }
      break;
    case estadoD:
      Serial.println("    ED: Usando energia das baterias    ");//ativa energia da bateria
      digitalWrite(porta_rele_energiaRua, LOW);
      digitalWrite(porta_rele_placaSolar, HIGH);
      tempo_uso();
      if(I == 0){
        estadoAtual = estadoB;
      }
      else if((float)carga_atual/capacidade_bateria < 0.5){
        estadoAtual = estadoC;
      }
      break;
    default:
      break;
  }
  Serial.println("==================");
  delay(3000);
}
