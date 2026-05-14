# Docs

Dokumentacja pomocnicza dla demonstratora `AR IoT ServiceBox`.

## Co jest tutaj opisane

- architektura komunikacji MQTT między `ESP32`, `Raspberry Pi` i klientem AR
- uproszczony kontrakt integracyjny dla `Quest 3`
- miejsce na dalsze notatki architektoniczne, diagramy i procedury testowe

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

## Powiązanie z kodem

- `firmware/` publikuje techniczną telemetrię urządzenia i odbiera komendy sterujące
- `server/` subskrybuje telemetrię, zapisuje log i tłumaczy kontrakt `Quest 3`
- `ansible/` przygotowuje `Raspberry Pi`, instaluje `Mosquitto` i wdraża serwer jako usługę `systemd`

## Czego jeszcze brakuje

Warto dodać w kolejnych iteracjach:

- diagram przepływu danych `ESP32 -> MQTT -> Python bridge -> Quest`
- opis procedury lokalnych testów integracyjnych
- checklistę wdrożenia na świeże `Raspberry Pi`
- opis konfiguracji brokera MQTT dla lokalnego developmentu
