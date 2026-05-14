# AR IoT ServiceBox

Monorepo dla demonstratora AR-IoT opartego o ESP32 i Raspberry Pi.

## Struktura

- `firmware/` - firmware ESP32 w PlatformIO
- `server/` - aplikacja Python dla lokalnego developmentu i Raspberry Pi
- `ansible/` - provisioning Raspberry Pi i deployment serwera
- `docs/` - dokumentacja projektu
- `.vscode/` - wspólne taski i rekomendowane rozszerzenia

## Kolejność prac

1. Szkielet repozytorium i narzędzi developerskich.
2. Test IO firmware bez MQTT.
3. MQTT i telemetria po stronie ESP32.
4. Serwer Python z CLI do obsługi urządzenia.
5. Automatyzacja wdrożenia przez Ansible.
