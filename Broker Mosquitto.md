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

