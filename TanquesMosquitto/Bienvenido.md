# Guía de Implementación

## Sistema de Control de Tanques con ESP32, JSN-04, MQTT y OTA

---

## 1. Descripción General

Este sistema permite **monitorear y controlar 3 tanques de agua de 5000 L** utilizando un **ESP32 DevKit V1**, sensores ultrasónicos **JSN‑04 en modo UART**, control de **bombas mediante relés**, visualización local con **display MAX7219**, y comunicación remota mediante **MQTT**, con capacidad de **actualización OTA**.

Está diseñado con criterios **industriales**: robustez, cableado largo, desacoplo por mensajes, y estados seguros ante fallos.

---

## 2. Arquitectura del Sistema

```
Sensores JSN‑04 (UART)
        │
        ▼
      ESP32
        │
        ├── Relé Bomba T1
        ├── Relé Bomba T2
        ├── Relé Bomba T3
        ├── Relé Sirena
        ├── Display MAX7219
        ├── Botones de selección
        └── WiFi → MQTT → Backend / Móvil
```

---

## 3. Componentes

### Electrónicos

- ESP32 DevKit V1
    
- 3 × Sensor ultrasónico JSN‑04 (modo UART)
    
- Módulo relé 4 canales con optoacopladores
    
- Display 7 segmentos MAX7219 (8 dígitos)
    
- 3 × Pulsadores
    
- Fuente 5 V / 12 V (según relés)
    

### Software

- Arduino IDE ≥ 2.3.x
    
- Core ESP32 instalado
    
- Broker Mosquitto MQTT
    
- Backend / Frontend MQTT (Node‑RED, app móvil, etc.)
    

---

## 4. Configuración del Sensor JSN‑04 (UART)

### Jumpers

|Jumper|Estado|Función|
|---|---|---|
|M2|CERRADO|Modo UART|
|M1|CERRADO (opcional)|UART sin RX|

### Protocolo

- El sensor **no mide automáticamente**
    
- Requiere enviar el comando:
    

```
0x55
```

- Respuesta: **2 bytes** (distancia en cm)
    

---

## 5. Pinout Recomendado

### UART Sensores

|Señal|ESP32|
|---|---|
|TX|GPIO17|
|RX|GPIO16|

### Relés

|Función|GPIO|
|---|---|
|Bomba T1|26|
|Bomba T2|27|
|Bomba T3|14|
|Sirena|12|

### Display MAX7219

|Señal|GPIO|
|---|---|
|DIN|23|
|CLK|18|
|CS|5|

### Botones

|Botón|GPIO|
|---|---|
|Tanque 1|32|
|Tanque 2|33|
|Tanque 3|25|

---

## 6. Estados de Tanque

Cada tanque puede estar en uno de los siguientes estados:

|Estado|Condición|
|---|---|
|LOW|Nivel bajo (arranca bomba)|
|NORMAL|Operación estable|
|FULL|Nivel alto (detiene bomba)|
|OVERFLOW|Rebalse (alarma + bloqueo)|

---

## 7. Lógica de Control (AUTO)

```
SI distancia > LOW_CM   → Bomba ON
SI distancia < FULL_CM  → Bomba OFF
SI distancia < OVER_CM  → Bomba OFF + Sirena ON
```

---

## 8. Display MAX7219

### Visualización

- `T‑1 3450` → Tanque 1, 3450 litros
    
- `LOW` → Nivel bajo
    
- `FULL` → Tanque lleno
    
- `OVR` → Rebalse
    

### Botones

- Botón 1 → Tanque 1
    
- Botón 2 → Tanque 2
    
- Botón 3 → Tanque 3
    

---

## 9. MQTT

### Broker

- Mosquitto
    
- Puerto: 1883
    
- Autenticación por usuario
    

### Tópicos

|Tópico|Dirección|Función|
|---|---|---|
|esp32/t1|PUB|Estado tanque 1|
|esp32/t2|PUB|Estado tanque 2|
|esp32/t3|PUB|Estado tanque 3|
|esp32/cmd|SUB|Comandos|
|esp32/ack|SUB|Reset alarmas|

---

## 10. OTA (Actualización Remota)

Permite actualizar el firmware **sin acceso físico** al equipo.

### Ventajas

- Actualización en campo
    
- Corrección de errores
    
- Nuevas funciones
    

### Requisitos

- ESP32 y PC en la misma red
    
- Arduino IDE → Puerto OTA
    

---

## 11. Seguridad y Fail‑Safe

- Rebalse bloquea bombas
    
- Sirena dedicada
    
- ACK remoto para reset
    
- Separación lógica sensores / actuadores
    

---

## 12. Puesta en Marcha

1. Verificar alimentación
    
2. Probar sensores por UART
    
3. Validar display
    
4. Ver MQTT local
    
5. Activar bombas sin carga
    
6. Simular niveles
    

---

## 13. Expansiones Futuras

- Logs históricos
    
- Persistencia en flash
    
- Curva real litros ↔ cm
    
- Watchdog
    
- Redundancia WiFi
    
- Dashboard SCADA
    

---

## 14. Notas Finales

Este sistema está pensado para **uso real**, no experimental. La arquitectura por mensajes, OTA y estados garantiza **mantenibilidad y escalabilidad**.

---

**Autor:** Oscar Campos  
**Asistente técnico:** ChatGPT