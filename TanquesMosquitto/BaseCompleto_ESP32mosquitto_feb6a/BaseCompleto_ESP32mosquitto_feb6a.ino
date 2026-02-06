#include <LedControl.h>
#include <HardwareSerial.h>

// ================= DISPLAY =================
#define DIN 23
#define CLK 18
#define CS  5
LedControl lc = LedControl(DIN, CLK, CS, 1);

// ================= BOTONES =================
#define BTN1 32
#define BTN2 33
#define BTN3 34

// ================= SENSORES =================
HardwareSerial jsn(2);
#define JSN_RX 16
#define JSN_TX 17

#define JSN1_EN 25
#define JSN2_EN 26
#define JSN3_EN 27

// ================= VARIABLES =================
uint16_t distancia[3] = {0, 0, 0};
uint16_t litros[3]    = {0, 0, 0};
uint8_t tanqueSeleccionado = 0; // 0,1,2

// ================= FUNCIONES =================
void enableSensor(uint8_t pin) {
  digitalWrite(JSN1_EN, LOW);
  digitalWrite(JSN2_EN, LOW);
  digitalWrite(JSN3_EN, LOW);
  digitalWrite(pin, HIGH);
  delay(80);
}

uint16_t leerDistancia() {
  jsn.flush();
  jsn.write(0x55);
  delay(60);

  if (jsn.available() >= 4) {
    uint8_t b[4];
    jsn.readBytes(b, 4);
    if (b[0] == 0xFF) {
      uint8_t chk = (b[0] + b[1] + b[2]) & 0xFF;
      if (chk == b[3]) {
        return (b[1] << 8) | b[2];
      }
    }
  }
  return 0;
}

uint16_t mmToLitros(uint16_t mm) {
  if (mm > 4000) mm = 4000;
  if (mm < 800)  mm = 800;
  return map(mm, 4000, 800, 0, 5000);
}

void mostrarTanque(uint8_t t) {
  lc.clearDisplay(0);

  // Formato: "T-1 5685"
  lc.setChar(0, 7, 'T', false);
  lc.setChar(0, 6, '-', false);
  lc.setDigit(0, 5, t + 1, false);
  lc.setChar(0, 4, ' ', false);

  int v = litros[t];
  lc.setDigit(0, 3, (v / 1000) % 10, false);
  lc.setDigit(0, 2, (v / 100)  % 10, false);
  lc.setDigit(0, 1, (v / 10)   % 10, false);
  lc.setDigit(0, 0, v % 10, false);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  jsn.begin(9600, SERIAL_8N1, JSN_RX, JSN_TX);

  pinMode(JSN1_EN, OUTPUT);
  pinMode(JSN2_EN, OUTPUT);
  pinMode(JSN3_EN, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  Serial.println("Sistema iniciado");
}

// ================= LOOP =================
void loop() {
  // ---- Lectura sensores ----
  enableSensor(JSN1_EN);
  distancia[0] = leerDistancia();

  enableSensor(JSN2_EN);
  distancia[1] = leerDistancia();

  enableSensor(JSN3_EN);
  distancia[2] = leerDistancia();

  for (int i = 0; i < 3; i++) {
    litros[i] = mmToLitros(distancia[i]);
  }

  // ---- Botones ----
  if (!digitalRead(BTN1)) tanqueSeleccionado = 0;
  if (!digitalRead(BTN2)) tanqueSeleccionado = 1;
  if (!digitalRead(BTN3)) tanqueSeleccionado = 2;

  mostrarTanque(tanqueSeleccionado);

  delay(300);
}
