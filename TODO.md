# TODO

Aktualny stan projektu jest już demonstracyjnie działający, ale przed domknięciem warto jeszcze wykonać poniższe zadania.

- [ ] Zweryfikować provisioning `Raspberry Pi` przez `Ansible` na świeżym systemie.
- [ ] Sprawdzić pełny przepływ `Quest 3` lub klient MQTT `->` broker `->` Python bridge `->` ESP32.
- [ ] Potwierdzić, że broker MQTT na `Raspberry Pi` jest osiągalny z docelowej sieci i że klient AR może publikować oraz subskrybować właściwe topiki.
- [ ] Sprawdzić zachowanie po restartach i zanikach połączenia:
  - restart `mosquitto`
  - restart usługi `ar-iot-server`
  - reboot `Raspberry Pi`
  - utrata i powrót Wi-Fi po stronie `ESP32`
- [ ] Podmienić placeholdery w `ansible-vault` na rzeczywiste sekrety wdrożeniowe i zaszyfrować je własnym hasłem vault.

