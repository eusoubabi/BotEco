#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// ===== CONFIGURAÇÃO MODO STA =====
const char* home_ssid = "Barbara Rodrigues's A52s";
const char* home_password = "trfg3246";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// ===== PINOS =====
#define IN1_ESQ_FRENTE 27
#define IN2_ESQ_FRENTE 26
#define IN3_ESQ_TRAS   25
#define IN4_ESQ_TRAS   33
#define IN1_DIR_FRENTE 32
#define IN2_DIR_FRENTE 19
#define IN3_DIR_TRAS   18
#define IN4_DIR_TRAS    5

// ===== MOVIMENTOS =====
void parar() {
  digitalWrite(IN1_ESQ_FRENTE, LOW);
  digitalWrite(IN2_ESQ_FRENTE, LOW);
  digitalWrite(IN3_ESQ_TRAS, LOW);
  digitalWrite(IN4_ESQ_TRAS, LOW);
  digitalWrite(IN1_DIR_FRENTE, LOW);
  digitalWrite(IN2_DIR_FRENTE, LOW);
  digitalWrite(IN3_DIR_TRAS, LOW);
  digitalWrite(IN4_DIR_TRAS, LOW);
}

void frente() {
  digitalWrite(IN1_ESQ_FRENTE, HIGH);
  digitalWrite(IN2_ESQ_FRENTE, LOW);
  digitalWrite(IN3_ESQ_TRAS, LOW);
  digitalWrite(IN4_ESQ_TRAS, HIGH);
  digitalWrite(IN1_DIR_FRENTE, LOW);
  digitalWrite(IN2_DIR_FRENTE, HIGH);
  digitalWrite(IN3_DIR_TRAS, LOW);
  digitalWrite(IN4_DIR_TRAS, HIGH);
}

void direita() {
  digitalWrite(IN1_ESQ_FRENTE, HIGH);
  digitalWrite(IN2_ESQ_FRENTE, LOW);
  digitalWrite(IN3_ESQ_TRAS, LOW);
  digitalWrite(IN4_ESQ_TRAS, HIGH);
  digitalWrite(IN1_DIR_FRENTE, HIGH);
  digitalWrite(IN2_DIR_FRENTE, LOW);
  digitalWrite(IN3_DIR_TRAS, HIGH);
  digitalWrite(IN4_DIR_TRAS, LOW);
}

void esquerda() {
  digitalWrite(IN1_ESQ_FRENTE, LOW);
  digitalWrite(IN2_ESQ_FRENTE, HIGH);
  digitalWrite(IN3_ESQ_TRAS, HIGH);
  digitalWrite(IN4_ESQ_TRAS, LOW);
  digitalWrite(IN1_DIR_FRENTE, LOW);
  digitalWrite(IN2_DIR_FRENTE, HIGH);
  digitalWrite(IN3_DIR_TRAS, LOW);
  digitalWrite(IN4_DIR_TRAS, HIGH);
}

void rotaA() {
  Serial.println("🚗 Executando ROTA A");
  frente(); delay(5000);
  parar(); delay(500);
}

void rotaB() {
  Serial.println("🚗 Executando ROTA B");
  frente(); delay(5000);
  parar(); delay(500);
  direita(); delay(2000);
  frente(); delay(3000);
  parar(); delay(500);
}

void rotaC() {
  Serial.println("🚗 Executando ROTA C");
  frente(); delay(3000);
  parar(); delay(500);
  esquerda(); delay(2000);
  frente(); delay(5000);
  parar(); delay(500);
}

// ===== MONITORAMENTO DE CONEXÃO =====
void verificarWiFi() {
  static unsigned long ultimoCheck = 0;
  if (millis() - ultimoCheck > 5000) { // verifica a cada 5s
    ultimoCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠️ Wi-Fi desconectado! Tentando reconectar...");
      WiFi.reconnect();
    }
  }
}

// ===== HTML =====
const char* htmlPage = R"html(
<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>Controle Wi-Fi - ESP32</title>
<style>
body { background:#111;color:#fff;text-align:center;font-family:Arial;}
button {padding:15px 30px;margin:10px;border:none;border-radius:8px;font-size:18px;cursor:pointer;}
.a{background-color:#007bff}.b{background-color:#28a745}.c{background-color:#ffc107;color:#000}.stop{background-color:#dc3545}
#status{margin-top:20px;font-size:20px;color:#0f0}
</style>
<script>
let socket;
function init(){
 socket=new WebSocket('ws://'+window.location.hostname+':81/');
 socket.onopen=()=>document.getElementById('status').innerText='✅ Conectado';
 socket.onclose=()=>document.getElementById('status').innerText='⚠️ Desconectado';
 socket.onmessage=(e)=>document.getElementById('status').innerText=e.data;
}
function sendCommand(cmd){
 if(socket&&socket.readyState===WebSocket.OPEN){
   socket.send(cmd);
   document.getElementById('status').innerText='Executando '+cmd+'...';
 }
}
window.onload=init;
</script></head>
<body>
<h2>Controle de Rotas - ESP32</h2>
<button class='a' onclick="sendCommand('rotaA')">Rota A</button>
<button class='b' onclick="sendCommand('rotaB')">Rota B</button>
<button class='c' onclick="sendCommand('rotaC')">Rota C</button>
<br><br><button class='stop' onclick="sendCommand('parar')">Parar</button>
<div id='status'>Aguardando conexão...</div></body></html>
)html";

// ===== WEBSOCKET =====
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  String msg = String((char*)data).substring(0, len);
  Serial.println("📡 Comando recebido via WebSocket: " + msg);
  if (msg == "rotaA") { rotaA(); webSocket.broadcastTXT("Rota A concluída!"); }
  else if (msg == "rotaB") { rotaB(); webSocket.broadcastTXT("Rota B concluída!"); }
  else if (msg == "rotaC") { rotaC(); webSocket.broadcastTXT("Rota C concluída!"); }
  else if (msg == "parar") { parar(); webSocket.broadcastTXT("Robô parado."); }
}

void onEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) handleWebSocketMessage(NULL, payload, length);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  pinMode(IN1_ESQ_FRENTE, OUTPUT); pinMode(IN2_ESQ_FRENTE, OUTPUT);
  pinMode(IN3_ESQ_TRAS, OUTPUT); pinMode(IN4_ESQ_TRAS, OUTPUT);
  pinMode(IN1_DIR_FRENTE, OUTPUT); pinMode(IN2_DIR_FRENTE, OUTPUT);
  pinMode(IN3_DIR_TRAS, OUTPUT); pinMode(IN4_DIR_TRAS, OUTPUT);
  parar();

  WiFi.mode(WIFI_STA);
  WiFi.begin(home_ssid, home_password);
  Serial.println("🔌 Conectando ao Wi-Fi...");
  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
    delay(1000); Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi conectado!");
    Serial.print("📶 IP local: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Falha ao conectar. Verifique a alimentação e credenciais.");
  }

  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(onEvent);

  Serial.println("🌐 Servidor Web e WebSocket iniciados!");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
  webSocket.loop();
  verificarWiFi();
}
