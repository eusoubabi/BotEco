#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ===============================
// CONFIGURAÇÃO WI-FI
// ===============================
const char* ssid = "Robo_Entregas";
const char* password = "senha_segura";

WebServer server(80);

// ===============================
// VARIÁVEIS DE ESTADO
// ===============================
String estadoRota = "parado";   // "andando", "chegou", "parado"

// ===============================
// PINAGEM
// ===============================
#define IN1_ESQ_F 27
#define IN2_ESQ_F 26
#define IN3_ESQ_T 25
#define IN4_ESQ_T 33

#define IN1_DIR_F 32
#define IN2_DIR_F 19
#define IN3_DIR_T 18
#define IN4_DIR_T 5

// ===============================
// FUNÇÕES DE MOVIMENTO
// ===============================
void parar() {
  digitalWrite(IN1_ESQ_F, LOW);
  digitalWrite(IN2_ESQ_F, LOW);
  digitalWrite(IN3_ESQ_T, LOW);
  digitalWrite(IN4_ESQ_T, LOW);
  digitalWrite(IN1_DIR_F, LOW);
  digitalWrite(IN2_DIR_F, LOW);
  digitalWrite(IN3_DIR_T, LOW);
  digitalWrite(IN4_DIR_T, LOW);
}

void frente() {
  digitalWrite(IN1_ESQ_F, HIGH); digitalWrite(IN2_ESQ_F, LOW);
  digitalWrite(IN3_ESQ_T, HIGH); digitalWrite(IN4_ESQ_T, LOW);

  digitalWrite(IN1_DIR_F, HIGH); digitalWrite(IN2_DIR_F, LOW);
  digitalWrite(IN3_DIR_T, HIGH); digitalWrite(IN4_DIR_T, LOW);
}

void tras() {
  digitalWrite(IN1_ESQ_F, LOW); digitalWrite(IN2_ESQ_F, HIGH);
  digitalWrite(IN3_ESQ_T, LOW); digitalWrite(IN4_ESQ_T, HIGH);

  digitalWrite(IN1_DIR_F, LOW); digitalWrite(IN2_DIR_F, HIGH);
  digitalWrite(IN3_DIR_T, LOW); digitalWrite(IN4_DIR_T, HIGH);
}

void direita() {
  digitalWrite(IN1_ESQ_F, HIGH); digitalWrite(IN2_ESQ_F, LOW);
  digitalWrite(IN3_ESQ_T, HIGH); digitalWrite(IN4_ESQ_T, LOW);

  digitalWrite(IN1_DIR_F, LOW); digitalWrite(IN2_DIR_F, HIGH);
  digitalWrite(IN3_DIR_T, LOW); digitalWrite(IN4_DIR_T, HIGH);
}

void esquerda() {
  digitalWrite(IN1_ESQ_F, LOW); digitalWrite(IN2_ESQ_F, HIGH);
  digitalWrite(IN3_ESQ_T, LOW); digitalWrite(IN4_ESQ_T, HIGH);

  digitalWrite(IN1_DIR_F, HIGH); digitalWrite(IN2_DIR_F, LOW);
  digitalWrite(IN3_DIR_T, HIGH); digitalWrite(IN4_DIR_T, LOW);
}

// ===============================
// ESTRUTURA DE PASSO
// ===============================
struct Passo {
  void (*movimento)();
  unsigned long duracao;
};

Passo rotaAtual[10];
int totalPassos = 0;
int passoAtual = 0;
unsigned long inicioPasso = 0;

// ===============================
// SISTEMA DE ROTAS
// ===============================
void iniciarRota(Passo passos[], int tamanho) {
  memcpy(rotaAtual, passos, sizeof(Passo) * tamanho);
  totalPassos = tamanho;

  passoAtual = 0;
  inicioPasso = millis();
  estadoRota = "andando";

  rotaAtual[0].movimento();
}

// ===============================
// ROTAS DEFINIDAS
// ===============================
Passo rotaA_ida[] = {
  {frente, 5000},
  {parar, 500},
  {direita, 5000},
  {parar, 500}
};

Passo rotaA_volta[] = {
  {direita, 1500}, // giro 180
  {parar, 300},
  {tras, 5000},
  {parar, 500},
  {direita, 5000},
  {parar, 500}
};

Passo rotaB_ida[] = {
  {frente, 5000},
  {parar, 500},
  {esquerda, 5000},
  {parar, 500}
};

Passo rotaB_volta[] = {
  {esquerda, 1500}, // giro 180
  {parar, 300},
  {tras, 5000},
  {parar, 500},
  {esquerda, 5000},
  {parar, 500}
};

// ===============================
// GERENCIAMENTO NÃO BLOQUEANTE
// ===============================
void loopRota() {
  if (estadoRota != "andando") return;

  if (millis() - inicioPasso >= rotaAtual[passoAtual].duracao) {

    passoAtual++;

    if (passoAtual >= totalPassos) {
      parar();
      estadoRota = "chegou";
      return;
    }

    inicioPasso = millis();
    rotaAtual[passoAtual].movimento();
  }
}

// ===============================
// ENDPOINTS WEB
// ===============================
void executarAIda() {
  iniciarRota(rotaA_ida, 4);
  server.send(200, "text/plain", "Rota A IDA iniciada");
}

void executarAVolta() {
  iniciarRota(rotaA_volta, 6);
  server.send(200, "text/plain", "Rota A VOLTA iniciada");
}

void executarBIda() {
  iniciarRota(rotaB_ida, 4);
  server.send(200, "text/plain", "Rota B IDA iniciada");
}

void executarBVolta() {
  iniciarRota(rotaB_volta, 6);
  server.send(200, "text/plain", "Rota B VOLTA iniciada");
}

void executarParar() {
  parar();
  estadoRota = "parado";
  server.send(200, "text/plain", "Robô parado");
}

void statusRota() {
  String json = "{\"estado\":\"" + estadoRota + "\"}";
  server.send(200, "application/json", json);
}

// ===============================
// SETUP
// ===============================
void setup() {
  Serial.begin(115200);

  pinMode(IN1_ESQ_F, OUTPUT);
  pinMode(IN2_ESQ_F, OUTPUT);
  pinMode(IN3_ESQ_T, OUTPUT);
  pinMode(IN4_ESQ_T, OUTPUT);

  pinMode(IN1_DIR_F, OUTPUT);
  pinMode(IN2_DIR_F, OUTPUT);
  pinMode(IN3_DIR_T, OUTPUT);
  pinMode(IN4_DIR_T, OUTPUT);

  parar();

  WiFi.softAP(ssid, password);

  server.on("/api/rota/a_ida", executarAIda);
  server.on("/api/rota/a_volta", executarAVolta);

  server.on("/api/rota/b_ida", executarBIda);
  server.on("/api/rota/b_volta", executarBVolta);

  server.on("/api/parar", executarParar);
  server.on("/api/status_rota", statusRota);

  server.begin();
}

// ===============================
// LOOP
// ===============================
void loop() {
  server.handleClient();
  loopRota();
}
