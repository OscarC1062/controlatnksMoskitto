- Montar un **broker Mosquitto**
- Conectar tu **ESP32 por MQTT**
- Ver mensajes en **Node-RED**
- Hacer un **ejemplo práctico real**


ESP32  ──►  Mosquitto (broker)  ◄──  PC / Node-RED / otro ESP32

1️⃣ Habilitar el broker MQTT (Mosquitto)

sudo apt update
sudo apt install mosquitto mosquitto-clients

### Verificar que corre

`sudo systemctl status mosquitto`

Debe decir: **active (running)** ✔️

## 2️⃣ Permitir conexiones (modo simple – laboratorio)

Edita:

`sudo nano /etc/mosquitto/mosquitto.conf`

Agrega **al final**:

`listener 1883 allow_anonymous true`

Reinicia:

`sudo systemctl restart mosquitto`

## 3️⃣ Ver mensajes desde un dispositivo (PC)

### Suscribirse a un topic

`mosquitto_sub -h localhost -t "esp32/#" -v`

Eso deja al dispositivo **escuchando todo** lo que envíe el ESP32.

Ejemplo:

`esp32/temperatura 24.6 esp32/estado ON`

## 4️⃣ Publicar desde el ESP32

Ejemplo mínimo en el ESP32:

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TU_WIFI";
const char* password = "CLAVE";

const char* mqtt_server = "192.168.1.10"; // IP del broker

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setServer(mqtt_server, 1883);

  while (!client.connected()) {
    client.connect("ESP32_1");
    delay(500);
  }

  client.publish("esp32/estado", "conectado");
}

void loop() {
  client.loop();
}

---

## 5️⃣ “Habilitar” un dispositivo (qué significa realmente)

Un dispositivo **queda habilitado** cuando:

1. ✔️ Puede conectarse al broker
2. ✔️ Tiene permiso (anon o usuario)
3. ✔️ Se suscribe a un topic válido
    
**No hay registro previo obligatorio**, solo:

- credenciales correctas
- topic permitido

---

## 6️⃣ Habilitación **con usuario y contraseña** (recomendado)

### Crear usuario MQTT

`sudo mosquitto_passwd -c /etc/mosquitto/passwd esp32`

### Configurar Mosquitto

`allow_anonymous false password_file /etc/mosquitto/passwd listener 1883`

Reinicia:

`sudo systemctl restart mosquitto`

### Conectarse desde PC

`mosquitto_sub -h localhost -u esp32 -P CLAVE -t "esp32/#"`

---

## 7️⃣ Suscribirse desde otro ESP32

Cambia el código:

`client.subscribe("esp32/#");`

Y define un callback:

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic);
  Serial.print(" -> ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

---

## 🧠 Resumen mental rápido

- **Broker**: autoriza conexiones
- **Topic**: define qué ves
- **Subscriber**: “ve” mensajes
- **Publisher**: los envía
- No hay “registro”, solo permisos

