#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <HardwareSerial.h>
#include <LedControl.h>

/* ================= CONFIG ================= */

const char* ssid = "PEDRO CAMPOS"
const char* password = "TONINOCAMPOS29";

const char* mqtt_server = "192.168.0.10";
const char* mqtt_user = "esp32";
const char* mqtt_pass = "tonino";

/* ================= OBJETOS ================= */

WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial JSN(2);
LedControl display = LedControl(DISP_DIN, DISP_CLK, DISP_CS, 1);

/* ================= ESTRUCTURAS ================= */

struct Tank {
  uint16_t distancia;
  uint16_t litros;
  bool low;
  bool full;
  bool overflow;
  bool pumpOn;
};

Tank tanks[3];

uint8_t selectedTank = 0;
bool sirenActive = false;

/* ================= CONSTANTES ================= */

#define CMD_JSN 0x55
#define LOW_CM  80
#define FULL_CM 15
#define OVER_CM 5

/* ================= FUNCIONES ================= */

uint16_t leerJSN() {
  uint8_t cmd = CMD_JSN;
  JSN.write(cmd);
  delay(60);

  if (JSN.available() >= 2) {
    uint8_t hi = JSN.read();
    uint8_t lo = JSN.read();
    return (hi << 8) | lo;
  }
  return 0;
}

uint16_t cmToLitros(uint16_t cm) {
  if (cm == 0 || cm > 300) return 0;
  return map(cm, 300, 0, 0, 5000);
}

void evaluarTanque(uint8_t i) {
  tanks[i].low = tanks[i].distancia > LOW_CM;
  tanks[i].full = tanks[i].distancia < FULL_CM;
  tanks[i].overflow = tanks[i].distancia < OVER_CM;
}

void controlarBombas() {
  for (int i = 0; i < 3; i++) {
    if (tanks[i].overflow) {
      tanks[i].pumpOn = false;
      sirenActive = true;
    } else if (tanks[i].low) {
      tanks[i].pumpOn = true;
    } else if (tanks[i].full) {
      tanks[i].pumpOn = false;
    }
  }

  digitalWrite(RELAY_T1, tanks[0].pumpOn);
  digitalWrite(RELAY_T2, tanks[1].pumpOn);
  digitalWrite(RELAY_T3, tanks[2].pumpOn);
  digitalWrite(RELAY_SIREN, sirenActive);
}

void actualizarDisplay() {
  display.clearDisplay(0);
  Tank& t = tanks[selectedTank];

  if (t.overflow) {
    display.setChar(0, 7, 'O', false);
    display.setChar(0, 6, 'V', false);
    display.setChar(0, 5, 'R', false);
  } else if (t.low) {
    display.setChar(0, 7, 'L', false);
    display.setChar(0, 6, 'O', false);
    display.setChar(0, 5, 'W', false);
  } else if (t.full) {
    display.setChar(0, 7, 'F', false);
    display.setChar(0, 6, 'U', false);
    display.setChar(0, 5, 'L', false);
    display.setChar(0, 4, 'L', false);
  } else {
    uint16_t l = t.litros;
    for (int i = 0; i < 4; i++) {
      display.setDigit(0, i, l % 10, false);
      l /= 10;
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (String(topic) == "esp32/ack") {
    sirenActive = false;
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32_TANQUES", mqtt_user, mqtt_pass)) {
      client.subscribe("esp32/ack");
    } else {
      delay(5000);
    }
  }
}

/* ================= SETUP ================= */

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_T1, OUTPUT);
  pinMode(RELAY_T2, OUTPUT);
  pinMode(RELAY_T3, OUTPUT);
  pinMode(RELAY_SIREN, OUTPUT);

  pinMode(BTN_T1, INPUT_PULLUP);
  pinMode(BTN_T2, INPUT_PULLUP);
  pinMode(BTN_T3, INPUT_PULLUP);

  JSN.begin(9600, SERIAL_8N1, JSN_RX, JSN_TX);

  display.shutdown(0, false);
  display.setIntensity(0, 8);
  display.clearDisplay(0);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);

  ArduinoOTA.begin();
}

/* ================= LOOP ================= */

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) reconnectMQTT();
  client.loop();

  for (int i = 0; i < 3; i++) {
    tanks[i].distancia = leerJSN();
    tanks[i].litros = cmToLitros(tanks[i].distancia);
    evaluarTanque(i);
    delay(100);
  }

  if (!digitalRead(BTN_T1)) selectedTank = 0;
  if (!digitalRead(BTN_T2)) selectedTank = 1;
  if (!digitalRead(BTN_T3)) selectedTank = 2;

  controlarBombas();
  actualizarDisplay();

  delay(500);
}
