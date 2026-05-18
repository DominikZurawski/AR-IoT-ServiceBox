# Docs

Dokumentacja pomocnicza dla demonstratora `AR IoT ServiceBox`.

## Aktualne dokumenty

### `mqtt-protocol.md`

Opisuje:

- topologię komunikacji MQTT
- techniczne tematy urządzenia:
  - `arbox/<DEVICE_ID>/telemetry`
  - `arbox/<DEVICE_ID>/command`
- tematy pośrednie dla klienta AR:
  - `arbox/<DEVICE_ID>/quest/telemetry`
  - `arbox/<DEVICE_ID>/quest/command`
- przykładowe payloady telemetrii i komend
- założenia translacji między kontraktem urządzenia i kontraktem klienta AR

### `quest3-integration.md`

Opisuje:

- punkt integracji aplikacji `Quest 3` z brokerem MQTT na `Raspberry Pi`
- uproszczony format telemetrii i komend używany przez klienta AR
- zakres odpowiedzialności warstwy bridge w serwerze Python
- ograniczenia obecnej wersji integracji

### `node-red-dashboard.md`

Opisuje:

- jak uruchomić `Node-RED` jako warstwę operatorską na `Raspberry Pi`
- jak zaimportować przykładowy flow demonstracyjny
- jak podejrzeć telemetrię i wysyłać komendy z prostego dashboardu

## Powiązanie z kodem

- `firmware/` publikuje techniczną telemetrię urządzenia i odbiera komendy sterujące
- `server/` subskrybuje telemetrię, zapisuje log i tłumaczy kontrakt `Quest 3`
- `ansible/` przygotowuje `Raspberry Pi`, instaluje `Mosquitto` i wdraża serwer jako usługę `systemd`
- `Node-RED` może działać opcjonalnie jako panel testowy i dashboard operatorski nad tym samym brokerem MQTT

## Czego jeszcze brakuje

Warto dodać w kolejnych iteracjach:

- diagram przepływu danych `ESP32 -> MQTT -> Python bridge -> Quest`
- opis procedury lokalnych testów integracyjnych
- checklistę wdrożenia na świeże `Raspberry Pi`
- opis konfiguracji brokera MQTT dla lokalnego developmentu
