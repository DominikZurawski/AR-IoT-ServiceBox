# Quest 3 Integration

## Założenie architektoniczne

Aplikacja na Quest 3 nie komunikuje się bezpośrednio z ESP32.

Punkt integracji:

- broker MQTT na Raspberry Pi
- opcjonalnie serwer Python na Raspberry Pi jako warstwa translacji i logowania

## Co subskrybuje klient Quest 3

Temat:

`arbox/<DEVICE_ID>/quest/telemetry`

Przykładowy payload:

```json
{
  "device_id": "ARBOX-001",
  "timestamp": 1620860,
  "fanReportedRPM": 0,
  "fanReportedPWM": 0,
  "fanSelected": "A",
  "sensorTemperature": 26.7,
  "sensorHumidity": 33.8,
  "systemState": "OK",
  "errorCode": ""
}
```

## Co publikuje klient Quest 3

Temat:

`arbox/<DEVICE_ID>/quest/command`

Przykład ustawienia prędkości:

```json
{
  "command": "set_fan_pwm",
  "fan": "selected",
  "value": 80
}
```

Przykład awarii:

```json
{
  "command": "simulate_fault",
  "fault": "fan_blocked"
}
```

## Odpowiedzialność Raspberry Pi

Raspberry Pi:

- odbiera telemetrię techniczną z ESP32
- tłumaczy ją do uproszczonego kontraktu Quest 3
- odbiera uproszczone komendy z Questa
- tłumaczy je do technicznego formatu komend dla ESP32
- publikuje przetłumaczone komendy na topic urządzenia

## Ograniczenia obecnej wersji

- `fan = "selected"` jest wspierane
- bezpośredni wybór konkretnego kanału `A/B` nie jest jeszcze wspierany po stronie firmware
- `fanReportedRPM` zależy od podłączonych tachometrów; przy wyłączonych tachometrach zwracane jest `0`

## Test bez ESP32

Scenariusz:

- `Raspberry Pi` uruchamia broker MQTT i bridge Python
- laptop publikuje komendy jak klient `Quest 3`
- laptop publikuje też przykładową telemetrię techniczną, żeby zasymulować `ESP32`
- laptop subskrybuje topiki, żeby potwierdzić translację w obie strony

### 1. Wdrożenie na Raspberry Pi

Z katalogu [ansible](/home/dominik/Dokumenty/Projekty/XR/ESP32_Raspberry/ansible):

```bash
ansible-playbook -i inventory/hosts.yml site.yml --limit rpi-servicebox --ask-vault-pass
```

Po wdrożeniu na Raspberry Pi sprawdź:

```bash
systemctl status mosquitto
systemctl status ar-iot-server
ss -ltnp | grep 1883
```

### 2. Nasłuch topiców z laptopa

Podstaw zmienne:

- `RPI_IP=192.168.50.10`
- `DEVICE_ID=ARBOX-001`

Nasłuch technicznego topicu komend:

```bash
mosquitto_sub -h "$RPI_IP" -t "arbox/$DEVICE_ID/command" -v
```

W osobnym terminalu nasłuch uproszczonej telemetrii dla `Quest 3`:

```bash
mosquitto_sub -h "$RPI_IP" -t "arbox/$DEVICE_ID/quest/telemetry" -v
```

### 3. Symulacja Questa z laptopa

Wyślij komendę uproszczoną:

```bash
mosquitto_pub -h "$RPI_IP" -t "arbox/$DEVICE_ID/quest/command" -m '{"command":"set_fan_pwm","fan":"selected","value":80}'
```

Oczekiwany wynik na topicu `arbox/$DEVICE_ID/command`:

```json
{"command":"set_fan_pwm","value":80}
```

Możesz tak samo sprawdzić:

```bash
mosquitto_pub -h "$RPI_IP" -t "arbox/$DEVICE_ID/quest/command" -m '{"command":"simulate_fault","fault":"fan_blocked"}'
mosquitto_pub -h "$RPI_IP" -t "arbox/$DEVICE_ID/quest/command" -m '{"command":"resume_auto_pwm"}'
```

### 4. Symulacja ESP32 z laptopa

Wyślij techniczną telemetrię:

```bash
mosquitto_pub -h "$RPI_IP" -t "arbox/$DEVICE_ID/telemetry" -m '{"device_id":"ARBOX-001","timestamp_ms":1620860,"system_state":"OK","temperature_env_c":26.7,"temperature_power_c":28.0,"humidity_pct":33.8,"fan_a_rpm":1200,"fan_b_rpm":900,"fan_pwm":204,"fan_select":true,"lid_open":false,"iot_onoff_pressed":false,"soft_reset_pressed":false,"sensor_ok":true,"wifi_connected":true,"mqtt_connected":true,"error_code":"","active_fault":""}'
```

Oczekiwany wynik na topicu `arbox/$DEVICE_ID/quest/telemetry`:

```json
{"device_id":"ARBOX-001","timestamp":1620860,"fanReportedRPM":1200,"fanReportedPWM":80,"fanSelected":"A","sensorTemperature":26.7,"sensorHumidity":33.8,"systemState":"OK","errorCode":""}
```

### 5. Co potwierdza taki test

- broker `Mosquitto` na `Raspberry Pi` przyjmuje połączenia z LAN
- usługa `ar-iot-server` działa i subskrybuje właściwe topiki
- komendy `Quest 3` są tłumaczone na techniczny kontrakt urządzenia
- telemetria techniczna jest tłumaczona na kontrakt `Quest 3`
- laptop może pełnić rolę zarówno testowego klienta `Quest 3`, jak i symulatora `ESP32`
