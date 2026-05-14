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
