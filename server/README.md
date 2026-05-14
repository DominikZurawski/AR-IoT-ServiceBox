# Server

Pythonowy serwer dla lokalnego developmentu i Raspberry Pi.

## Wymagania

- Python 3.12
- lokalny broker MQTT, np. `mosquitto`

## Uruchomienie

Z katalogu `server/`:

1. Utwórz środowisko: `python3 -m venv .venv`
2. Aktywuj środowisko.
3. Zainstaluj zależności: `pip install -r requirements.txt`
4. Ustaw zmienne środowiskowe:
   - `AR_IOT_MQTT_HOST`
   - opcjonalnie `AR_IOT_MQTT_PORT`, `AR_IOT_MQTT_USERNAME`, `AR_IOT_MQTT_PASSWORD`
5. Uruchom monitor telemetrii: `python -m ar_iot_server monitor`

## Status

Na tym etapie katalog zawiera:

- monitor MQTT subskrybujący `arbox/+/telemetry`
- logowanie telemetrii na konsolę i do `logs/telemetry.jsonl`
- CLI do listy urządzeń na podstawie logu
- CLI do komend:
  - `set-fan-pwm`
  - `simulate-fault`
  - `resume-auto-pwm`
- bridge kontraktu Quest 3:
  - telemetria uproszczona na `arbox/<DEVICE_ID>/quest/telemetry`
  - komendy uproszczone na `arbox/<DEVICE_ID>/quest/command`

## Przykłady CLI

Uruchom monitor:

`python -m ar_iot_server monitor`

Wyświetl urządzenia z logu:

`python -m ar_iot_server devices`

Ustaw PWM:

`python -m ar_iot_server set-fan-pwm servicebox-lite-001 80`

Ustaw PWM przez kontrakt Quest:

`python -m ar_iot_server quest-set-fan-pwm servicebox-lite-001 80`

Wywołaj awarię:

`python -m ar_iot_server simulate-fault servicebox-lite-001 overtemperature`

Wyczyść awarię:

`python -m ar_iot_server simulate-fault servicebox-lite-001 clear`

Wznów automatyczne PWM:

`python -m ar_iot_server resume-auto-pwm servicebox-lite-001`
