# Firmware

Katalog na firmware dla `ESP32 DevKitC / ESP-WROOM-32D` oparty o PlatformIO.

## Wymagania

- VS Code
- rozszerzenie PlatformIO IDE
- sterowniki USB do używanej płytki ESP32

## Podstawowe komendy

Z katalogu `firmware/`:

- build: `pio run`
- upload: `pio run -t upload`
- serial monitor: `pio device monitor`

## Status

Na tym etapie projekt zawiera firmware do walidacji hardware `AR-IoT ServiceBox Lite` oraz pierwszą integrację z MQTT:

- inicjalizację magistrali I2C na `GPIO21` i `GPIO22`
- skan I2C i próbę komunikacji z czujnikiem `SHT31` pod `0x44` lub `0x45`
- sterowanie PWM wentylatorów na `GPIO25` i `GPIO26`
- odczyt tachometrów wentylatorów z `GPIO34` i `GPIO35`
- odczyt wejść cyfrowych na `GPIO32`, `GPIO33`, `GPIO27`, `GPIO14`
- sterowanie diodą statusu na `GPIO13`
- strukturę `ServiceBoxTelemetry` i ramkę JSON wypisywaną przez `Serial` co `1 s`
- publikację telemetrii MQTT na `arbox/<DEVICE_ID>/telemetry`
- subskrypcję komend MQTT na `arbox/<DEVICE_ID>/command`
- prostą logikę stanu `OK/WARNING/ERROR`, `error_code` i fault injection

Struktura kodu:

- `src/devices/` - moduły urządzeń sprzętowych (`Sht31Sensor`, `FanController`, `InputPanel`, `StatusLed`)
- `src/services/` - moduły logiki aplikacyjnej (`TelemetryService`, `MqttService`)
- `src/AppConfig.h` - wspólna konfiguracja pinów, progów i sieci
- `src/TelemetryTypes.h` - wspólne struktury danych
- `src/main.cpp` - tylko inicjalizacja i orkiestracja pętli

## Test sprzętowy

Po wgraniu firmware i uruchomieniu `pio device monitor`:

- firmware wypisze znalezione urządzenia I2C i czy `SHT31` odpowiada po I2C
- LED na `GPIO13` miga wolno w stanie spoczynku i szybciej, gdy aktywne jest wejście cyfrowe
- PWM wentylatorów przechodzi sekwencją `0 -> 128 -> 255 -> 0`
- co `1 s` pojawia się ramka JSON z telemetrią oraz próba publikacji MQTT

## Konfiguracja MQTT

Przed buildem:

1. skopiuj `src/AppSecrets.example.h` do `src/AppSecrets.h`
2. uzupełnij w `src/AppSecrets.h`:
   - `DEVICE_ID`
   - `WIFI_SSID`
   - `WIFI_PASSWORD`
   - `MQTT_HOST`
   - opcjonalnie `MQTT_USERNAME` i `MQTT_PASSWORD`
3. w `src/AppConfig.h` zostaw parametry sprzętowe i progi pracy

Plik `src/AppSecrets.h` jest lokalny i jest ignorowany przez Git.

Opcjonalnie ustaw w `src/AppConfig.h`:

- `ENABLE_TACHOMETERS = true`, gdy wejścia tachometru są już fizycznie podłączone

Tematy MQTT:

- telemetry: `arbox/<DEVICE_ID>/telemetry`
- command: `arbox/<DEVICE_ID>/command`

Przykładowe komendy:

```json
{"command":"set_fan_pwm","value":80}
```

```json
{"command":"simulate_fault","fault":"overtemperature"}
```

```json
{"command":"simulate_fault","fault":"fan_blocked"}
```

```json
{"command":"simulate_fault","fault":"clear"}
```

```json
{"command":"resume_auto_pwm"}
```

Przyciski są skonfigurowane jako `INPUT_PULLUP`, więc stan aktywny to zwarcie pinu do `GND`.
