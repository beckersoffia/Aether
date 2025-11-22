#include <Wire.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

// ==== Definições de pinos e constantes ====
#define PINO_MQ135 A0
#define PINO_MQ2   A1
#define DHTPIN     7
#define DHTTYPE    DHT22

//SD-CARD
File myFile;
int pinoSS = 53;


DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);

// ==== Variáveis globais ====
float temperatura = 0.0;
float umidade = 0.0;

void setup() {
  // Inicializa comunicação serial
  Serial.begin(9600);

  // Configura pinos
  pinMode(PINO_MQ135, INPUT);
  pinMode(PINO_MQ2, INPUT);
  pinMode(DHTPIN, INPUT);

  // Inicializa sensores
  dht.begin();
  
  // Inicializa display 
  lcd.init();
  lcd.backlight();

  //Inicializa SD Card
  pinMode(pinoSS, OUTPUT);
   
  if (SD.begin()) { // Inicializa o SD Card
    Serial.println("SD Card pronto para uso.");
  }
  else {
    Serial.println("Falha na inicialização do SD Card.");
    return;
  }

  myFile = SD.open("dados.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("CO2;C0;T;U");
    myFile.close();
  }else {     // Se o Arquivo não abrir
    Serial.println("Erro ao Abrir Arquivo .txt");
  }
}

void loop() {
  // ==== Leitura dos sensores ====
  int leituraMQ135 = analogRead(PINO_MQ135);
  int leituraMQ2 = analogRead(PINO_MQ2);
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();

  // Verifica se leitura do DHT é válida
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println(F("Falha na leitura do sensor DHT!"));
    return;
  }

  

  // ==== Exibe no Serial Monitor ====
  Serial.print("MQ-135 (CO2): ");
  Serial.println(leituraMQ135);
  
  Serial.print("MQ-2 (CO): ");
  Serial.println(leituraMQ2);
  
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");


  myFile = SD.open("dados.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(leituraMQ135);
    myFile.print(";");
    myFile.print(leituraMQ2);
    myFile.print(";");
    myFile.print(temperatura);
    myFile.print(";");
    myFile.println(umidade);
    myFile.close();
    Serial.println("Dados gravados!");
  }else {
    Serial.println("Erro ao Abrir Arquivo .txt");
  }


  // Linha 1: MQ-135 e MQ-2
  lcd.setCursor(0, 0);
  lcd.print("CO2:");
  lcd.print(leituraMQ135);

  lcd.setCursor(9, 0);
  lcd.print("CO:");
  lcd.print(leituraMQ2);

  // Linha 2: Temperatura e Umidade
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperatura);
  lcd.print("C");

  lcd.setCursor(9, 1);
  lcd.print("U:");
  lcd.print(umidade);
  lcd.print("%");

  // ==== Intervalo de atualização ====
  delay(1000);
}
