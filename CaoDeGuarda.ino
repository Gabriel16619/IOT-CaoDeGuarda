#include <WiFi.h>
#include <HTTPClient.h>

// ===== WIFI =====
#define WIFI_SSID "IoT-B08"
#define WIFI_PASS "12345678"

// ===== LED =====
#define LED_VERDE   5
#define LED_VERMELHO 19

// ===== API =====
const char* urlHealthCheck = "https://jsonplaceholder.typicode.com/todos/1";

const unsigned long INTERVALO_MS = 15000;
unsigned long ultimaChecagem = 0;

void sinalOk() {
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
  Serial.println("[STATUS] Servidor OK - LED VERDE aceso");
}

void sinalErro(const char* motivo) {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, HIGH);
  Serial.print("[ERRO] ");
  Serial.println(motivo);
}

void reconectarWifi() {
  if (WiFi.status() == WL_CONNECTED) return;

  sinalErro("WiFi desconectado! Tentando reconectar...");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Reconectado!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Falha na reconexão.");
  }
}

void verificarServidor() {
  HTTPClient http;
  http.begin(urlHealthCheck);
  http.setTimeout(10000);

  int httpCode = http.GET();

  if (httpCode == 200) {
    sinalOk();
  } else if (httpCode > 0) {
    char msg[50];
    snprintf(msg, sizeof(msg), "Servidor retornou código %d", httpCode);
    sinalErro(msg);
  } else {
    
    sinalErro("Falha ao conectar ao servidor (sem resposta)");
  }

  Serial.print("[HTTP] Código retornado: ");
  Serial.println(httpCode);

  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  sinalErro("Inicializando...");

  Serial.print("[WiFi] Conectando a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Conectado!");
  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());

  verificarServidor();
  ultimaChecagem = millis();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    WiFi.reconnect();
    delay(5000);
    return;
  }

  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);

  if (millis() - ultimaChecagem >= INTERVALO_MS) {
    ultimaChecagem = millis();
    verificarServidor();
  }
}






