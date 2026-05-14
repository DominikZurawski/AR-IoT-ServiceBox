# MQTT Protocol

## Topologia

- `ESP32 -> Raspberry Pi broker`: telemetria techniczna
- `Raspberry Pi broker -> ESP32`: komendy techniczne
- `Quest 3 -> Raspberry Pi broker`: komendy uproszczone
- `Raspberry Pi broker -> Quest 3`: telemetria uproszczona

Raspberry Pi pełni rolę pośrednika i tłumacza między kontraktem urządzenia a kontraktem klienta AR.

## Tematy techniczne

- telemetry: `arbox/<DEVICE_ID>/telemetry`
- command: `arbox/<DEVICE_ID>/command`

## Tematy Quest 3

- telemetry: `arbox/<DEVICE_ID>/quest/telemetry`
- command: `arbox/<DEVICE_ID>/quest/command`

## Telemetria techniczna z ESP32

Przykład:

```json
{
  "device_id": "servicebox-lite-001",
  "timestamp_ms": 1620860,
  "system_state": "OK",
  "temperature_env_c": 26.7,
  "temperature_power_c": 28.0,
  "humidity_pct": 33.8,
  "fan_a_rpm": 0,
  "fan_b_rpm": 0,
  "fan_pwm": 0,
  "fan_select": true,
  "lid_open": false,
  "iot_onoff_pressed": false,
  "soft_reset_pressed": false,
  "sensor_ok": true,
  "wifi_connected": true,
  "mqtt_connected": true,
  "error_code": "",
  "active_fault": ""
}
```

## Komendy techniczne do ESP32

Ustawienie PWM:

```json
{
  "command": "set_fan_pwm",
  "value": 80
}
```

Symulacja awarii:

```json
{
  "command": "simulate_fault",
  "fault": "fan_blocked"
}
```

Kasowanie awarii:

```json
{
  "command": "simulate_fault",
  "fault": "clear"
}
```

Powrót do auto PWM:

```json
{
  "command": "resume_auto_pwm"
}
```

## Telemetria uproszczona dla Quest 3

Temat:

`arbox/<DEVICE_ID>/quest/telemetry`

Przykład:

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

Uwagi:

- `fanSelected = "A"` gdy `fan_select = true`
- `fanSelected = "B"` gdy `fan_select = false`
- `fanReportedRPM` dotyczy aktualnie wybranego wentylatora
- `fanReportedPWM` jest przeliczane do zakresu `0..100`
- `device_id` jest tłumaczone z `servicebox-lite-001` na `ARBOX-001`

## Komendy uproszczone z Quest 3

Temat:

`arbox/<DEVICE_ID>/quest/command`

Ustawienie PWM:

```json
{
  "command": "set_fan_pwm",
  "fan": "selected",
  "value": 80
}
```

Bridge na Raspberry Pi tłumaczy to do:

```json
{
  "command": "set_fan_pwm",
  "value": 80
}
```

Obsługiwane komendy uproszczone:

```json
{
  "command": "simulate_fault",
  "fault": "overtemperature"
}
```

```json
{
  "command": "simulate_fault",
  "fault": "fan_blocked"
}
```

```json
{
  "command": "simulate_fault",
  "fault": "clear"
}
```

```json
{
  "command": "resume_auto_pwm"
}
```
