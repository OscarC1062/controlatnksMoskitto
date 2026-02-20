### Iniciar mosquitto

** CLAVE tonino *******
*----------REINICIAR
sudo systemctl restart mosquitto
sudo systemctl reload mosquitto (SIN REBOOT)

*----------VERIFICAR
sudo systemctl status mosquitto

*----------LOGS
journalctl -xeu mosquitto.service
journalctl -u mosquitto -f

*----------DETENER
sudo systemctl stop mosquitto

*----------TERMINAL DE MENSAJES
mosquitto_sub -h localhost -u esp32 -P TU_CLAVE_MQTT -t "#" -v

*----------MENSAJES
mosquitto_pub -h localhost -u esp32 -P TU_CLAVE_MQTT -t test/mosquitto -m "OK"


# 🧭 PARTE 1 — Habilitar Mosquitto (Broker MQTT)

## ✅ Paso 1 — Instalar Mosquitto

### 🔹 En Windows

1. Descargar desde el sitio oficial de Eclipse Mosquitto
    
2. Instalar con opciones por defecto
    
3. Activar como servicio
    

### 🔹 En Linux (Ubuntu/Debian)

`sudo apt update sudo apt install mosquitto mosquitto-clients`

---

## ✅ Paso 2 — Verificar que esté corriendo

En consola:

`mosquitto -v`

Si ves:

`Opening ipv4 listen socket on port 1883`

✔ El broker está activo.

---

## ✅ Paso 3 — Habilitar usuario y contraseña (recomendado)

Crear archivo de usuarios:

`sudo mosquitto_passwd -c /etc/mosquitto/passwd esp32`

Te pedirá contraseña.

Editar archivo config:

`sudo nano /etc/mosquitto/mosquitto.conf`

Agregar:

`allow_anonymous false password_file /etc/mosquitto/passwd listener 1883`

Reiniciar:

`sudo systemctl restart mosquitto`

---

# 🧭 PARTE 2 — Probar MQTT desde PC

Antes de usar el ESP32, probamos el broker.

### Terminal 1 (Suscriptor)

`mosquitto_sub -h 192.168.1.10 -t esp32/# -u esp32 -P TU_CLAVE -v`

### Terminal 2 (Publicador)

`mosquitto_pub -h 192.168.1.10 -t esp32/test -m "Hola Mundo" -u esp32 -P TU_CLAVE`

Si en la Terminal 1 aparece el mensaje ✔  
Tu broker está funcionando.

---

# 🧭 PARTE 3 — Configurar el ESP32

En el sketch modificar:

`const char* ssid = "TU_WIFI"; const char* password = "TU_CLAVE_WIFI";  const char* mqtt_server = "192.168.1.10"; const char* mqtt_user = "esp32"; const char* mqtt_pass = "TU_CLAVE";`

⚠ Importante:

- La IP debe ser la del equipo donde corre Mosquitto
    
- ESP32 y broker deben estar en la misma red
    

---

# 🧭 PARTE 4 — Verificar conexión del ESP32

Cuando el ESP32 arranca:

1. Se conecta a WiFi
    
2. Ejecuta `client.connect(...)`
    
3. Se suscribe a:
    

`esp32/ack`

---

## 🧪 Prueba real con tu sistema

### En PC:

`mosquitto_pub -h 192.168.1.10 -t esp32/ack -m "reset" -u esp32 -P TU_CLAVE`

📌 Resultado esperado:

- La sirena se apaga
    
- El ESP32 recibe el mensaje
    

---

# 🧭 PARTE 5 — Ver mensajes del ESP32

Si agregás publicación en el loop (ejemplo):

`client.publish("esp32/t1", String(tanks[0].litros).c_str());`

Podés monitorear con:

`mosquitto_sub -h 192.168.1.10 -t esp32/# -u esp32 -P TU_CLAVE -v`

Y verás:

`esp32/t1 3450`

---

# 🛠 Checklist Rápido de Diagnóstico

|Problema|Causa probable|
|---|---|
|No conecta WiFi|SSID o clave mal|
|No conecta MQTT|IP incorrecta|
|Timeout MQTT|Firewall bloqueando 1883|
|Se conecta pero no recibe|No está suscripto|

---

# 🎯 Flujo Final Esperado

`ESP32 → WiFi → Mosquitto → Suscriptores PC/App → Mosquitto → ESP32`

---

Si querés, el próximo paso puede ser:

- 📊 Integrarlo con Node-RED
    
- 📱 Integrarlo con app móvil
    
- 📈 Guardar históricos
    
- 🔐 Activar MQTT sobre TLS (seguridad industrial)
    

Cuando lo pruebes, contame:

> ¿Conecta a la primera o te pelea la red? 😄