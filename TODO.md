# TODO

Aktualny stan projektu jest już demonstracyjnie działający, ale przed domknięciem warto jeszcze wykonać poniższe zadania.

## Priorytet

- [ ] Zweryfikować provisioning `Raspberry Pi` przez `Ansible` na świeżym systemie.
- [ ] Sprawdzić pełny przepływ `Quest 3` lub klient MQTT `->` broker `->` Python bridge `->` ESP32.
- [ ] Potwierdzić, że broker MQTT na `Raspberry Pi` jest osiągalny z docelowej sieci i że klient AR może publikować oraz subskrybować właściwe topiki.
- [ ] Sprawdzić zachowanie po restartach i zanikach połączenia:
  - restart `mosquitto`
  - restart usługi `ar-iot-server`
  - reboot `Raspberry Pi`
  - utrata i powrót Wi-Fi po stronie `ESP32`
- [ ] Podmienić placeholdery w `ansible-vault` na rzeczywiste sekrety wdrożeniowe i zaszyfrować je własnym hasłem vault.

## Jakość

- [ ] Rozszerzyć testy `server/`, jeśli pojawią się kolejne ścieżki biznesowe lub REST API.
- [ ] Dopisać checklistę finalnego testu demonstratora do `docs/`.

## Decyzje

- [ ] Potwierdzić docelowy tryb uwierzytelnienia brokera MQTT:
  - anonimowy dostęp w zaufanej sieci
  - login/hasło dla klientów MQTT

## Poza zakresem na teraz

- lokalny `docker-compose.yml` dla `mosquitto` został świadomie pominięty, bo aktualny plan zakłada testy na brokerze uruchamianym bezpośrednio na `Raspberry Pi`
