#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configurações de WiFi
const char* ssid = "SUA_REDE_WIFI";
const char* password = "SUA_SENHA";
const char* serverUrl = "http://IP_DO_SERVIDOR:3000/api/commands";

// Definição dos pinos (ajuste conforme seu hardware)
const int pinMotorA1 = 26;  // Motor A pino 1
const int pinMotorA2 = 27;  // Motor A pino 2
const int pinMotorB1 = 14;  // Motor B pino 1
const int pinMotorB2 = 12;  // Motor B pino 2
const int pinEnableA = 25;  // PWM Motor A
const int pinEnableB = 33;  // PWM Motor B

// Velocidade dos motores (0-255)
const int motorSpeed = 200;

void setup() {
  Serial.begin(115200);
  
  // Configura pinos dos motores
  pinMode(pinMotorA1, OUTPUT);
  pinMode(pinMotorA2, OUTPUT);
  pinMode(pinMotorB1, OUTPUT);
  pinMode(pinMotorB2, OUTPUT);
  pinMode(pinEnableA, OUTPUT);
  pinMode(pinEnableB, OUTPUT);
  
  // Inicia com motores parados
  stopMotors();
  
  // Conecta ao WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      processCommands(payload);
    } else {
      Serial.printf("Erro na requisição: %d\n", httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado - Tentando reconectar...");
    WiFi.reconnect();
  }
  
  delay(100); // Intervalo entre requisições
}

void processCommands(String payload) {
  Serial.println("Payload recebido: " + payload);
  
  // Para todos os motores inicialmente
  stopMotors();
  
  // Analisa o JSON recebido
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  JsonArray commands = doc["commands"];
  
  // Processa cada comando
  for (String cmd : commands) {
    executeCommand(cmd);
  }
}

void executeCommand(String command) {
  Serial.println("Executando: " + command);
  
  // Configuração PWM
  analogWrite(pinEnableA, motorSpeed);
  analogWrite(pinEnableB, motorSpeed);
  
  if (command == "up") {
    // Frente
    digitalWrite(pinMotorA1, HIGH);
    digitalWrite(pinMotorA2, LOW);
    digitalWrite(pinMotorB1, HIGH);
    digitalWrite(pinMotorB2, LOW);
  } 
  else if (command == "down") {
    // Trás
    digitalWrite(pinMotorA1, LOW);
    digitalWrite(pinMotorA2, HIGH);
    digitalWrite(pinMotorB1, LOW);
    digitalWrite(pinMotorB2, HIGH);
  }
  else if (command == "left") {
    // Esquerda
    digitalWrite(pinMotorA1, LOW);
    digitalWrite(pinMotorA2, HIGH);
    digitalWrite(pinMotorB1, HIGH);
    digitalWrite(pinMotorB2, LOW);
  }
  else if (command == "right") {
    // Direita
    digitalWrite(pinMotorA1, HIGH);
    digitalWrite(pinMotorA2, LOW);
    digitalWrite(pinMotorB1, LOW);
    digitalWrite(pinMotorB2, HIGH);
  }
  else if (command == "up-left") {
    // Diagonal frente-esquerda
    digitalWrite(pinMotorA1, LOW);
    digitalWrite(pinMotorA2, LOW);
    digitalWrite(pinMotorB1, HIGH);
    digitalWrite(pinMotorB2, LOW);
  }
  else if (command == "up-right") {
    // Diagonal frente-direita
    digitalWrite(pinMotorA1, HIGH);
    digitalWrite(pinMotorA2, LOW);
    digitalWrite(pinMotorB1, LOW);
    digitalWrite(pinMotorB2, LOW);
  }
  else if (command == "down-left") {
    // Diagonal trás-esquerda
    digitalWrite(pinMotorA1, LOW);
    digitalWrite(pinMotorA2, HIGH);
    digitalWrite(pinMotorB1, LOW);
    digitalWrite(pinMotorB2, LOW);
  }
  else if (command == "down-right") {
    // Diagonal trás-direita
    digitalWrite(pinMotorA1, LOW);
    digitalWrite(pinMotorA2, LOW);
    digitalWrite(pinMotorB1, LOW);
    digitalWrite(pinMotorB2, HIGH);
  }
  else if (command == "rotate-left") {
    // Rotação esquerda
    digitalWrite(pinMotorA1, LOW);
    digitalWrite(pinMotorA2, HIGH);
    digitalWrite(pinMotorB1, HIGH);
    digitalWrite(pinMotorB2, LOW);
  }
  else if (command == "rotate-right") {
    // Rotação direita
    digitalWrite(pinMotorA1, HIGH);
    digitalWrite(pinMotorA2, LOW);
    digitalWrite(pinMotorB1, LOW);
    digitalWrite(pinMotorB2, HIGH);
  }
}

void stopMotors() {
  digitalWrite(pinMotorA1, LOW);
  digitalWrite(pinMotorA2, LOW);
  digitalWrite(pinMotorB1, LOW);
  digitalWrite(pinMotorB2, LOW);
  analogWrite(pinEnableA, 0);
  analogWrite(pinEnableB, 0);
}