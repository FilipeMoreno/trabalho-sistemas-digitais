#include "DHT.h"
#include <LiquidCrystal.h>

/*******************************************************************************
*
*    Controle de temperatura com sensor, alarme e display
*    Filipe Moreno - RA: 111839
*    Leonardo 
*
*******************************************************************************/

// Configurações do Sensor
#define DHTTYPE DHT11
#define DHTPIN A1
DHT dht(DHTPIN, DHTTYPE);

// Configurações do Display
int rs = 12, en = 13, d4 = 10, d5 = 9, d6 = 6, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int sensorPin = A0, led = 6;

#define LedVermelho 4 // Pino do Led
#define Buzzer 11 // Pino do Buzzer
#define botao 8 // Pino do botão

float temperatura = 0; 
float umidade = 0;

int estadoBotao = 0;
int modo = 0;

int tempMin = 2;
int tempMax = 8;

//****************************************************************************

void setup() {
  Serial.begin(9600);         // taxa comunicação da placa com o computador
  analogReference(INTERNAL);

  pinMode(LedVermelho, OUTPUT);

  pinMode(botao, INPUT);

  lcd.begin(16, 2);

  dht.begin();
}

//****************************************************************************

void loop() {

  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();
  float hic = dht.computeHeatIndex(temperatura, umidade, false);

  estadoBotao = digitalRead(botao);

  if (estadoBotao == HIGH) {
    if (modo == 0) {
      Serial.println("Modo ambiente ativo.");
      tempMin = 15;
      tempMax = 30;
      modo = 1;
    } else {
      Serial.println("Modo geladeira ativo.");
      tempMin = 2;
      tempMax = 8;
      modo = 0;
    }
  }

  Serial.print("Temp min: ");
  Serial.print(tempMin);
  Serial.print(F("°C \t"));
  Serial.print("Temp max: ");
  Serial.print(tempMax);
  Serial.println(F("°C "));

  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Failed to read from DHT");
    lcd.begin(16, 2);
    lcd.print("Falha na leitura");
  } else {

    Serial.print("Temperatura atual = ");
    Serial.print(temperatura);
    Serial.print(F("°C "));

    Serial.print("Umidade atual = "); 
    Serial.print(umidade);
    Serial.println(" %");

    Serial.print(F("Índice de calor: "));
    Serial.print(hic);
    Serial.println(F("°C "));

    lcd.begin(16, 2);
    lcd.print("Temp: ");
    lcd.print(hic);
    lcd.print(" *C");
    lcd.setCursor(0, 1);

    if (modo == 0) {
      lcd.print("Modo: Geladeira");
    } else {
      lcd.print("Modo: Ambiente");
    }

    if (temperatura < tempMin || temperatura > tempMax) {
      lcd.begin(16, 2);
      lcd.print("ALERTA: ");
      lcd.print(hic);
      lcd.print(" *C");
      lcd.setCursor(0, 1);
      if (modo == 0) {
        lcd.print("Modo: Geladeira");
      } else {
        lcd.print("Modo: Ambiente");
      }

      tone(Buzzer, 1000, 1000);

      for (int i = 0; i < 80; i++) {
        digitalWrite(Buzzer, HIGH);
        delay(1);  //wait for 1ms
        digitalWrite(Buzzer, LOW);
        delay(1);  //wait for 1ms
      }

      digitalWrite(LedVermelho, HIGH);
    } else {
      digitalWrite(Buzzer, LOW);
      digitalWrite(LedVermelho, LOW);
    }
  }

  delay(2000);
}