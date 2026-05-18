# Ansible

Automatyzacja przygotowania Raspberry Pi i wdrożenia serwera.

## Zakres

- rola `raspberry_pi_base` instaluje bazowe pakiety i uruchamia Mosquitto
- rola `raspberry_pi_base` wystawia Mosquitto na porcie `1883` w LAN
- rola `ar_iot_server` klonuje repozytorium, tworzy `venv`, instaluje zależności i uruchamia `python -m ar_iot_server monitor` jako usługę `systemd`
- opcjonalna rola `node_red` instaluje `Node-RED`, `FlowFuse Dashboard` i przykładowy flow demonstracyjny

## Wymagane zmienne

W `inventory/hosts.yml` ustaw co najmniej:

- `ansible_host`
- `ansible_user`
- `ar_iot_server_repo_url`
- opcjonalnie `ar_iot_server_repo_version`

Jeśli broker lub serwer mają używać innych parametrów niż domyślne, ustaw też:

- `ar_iot_server_mqtt_host`
- `ar_iot_server_mqtt_port`

Opcjonalnie dla `Node-RED`:

- `node_red_enabled`

## Sekrety w `ansible-vault`

Sekrety wdrożeniowe trzymaj w `group_vars/all/vault.yml`.

Repo zawiera:

- `group_vars/all/vault.yml` - zaszyfrowany placeholder
- `group_vars/all/vault.yml.example` - jawny przykład struktury

Przed użyciem:

1. podmień placeholdery w `group_vars/all/vault.yml` na własne wartości przez `ansible-vault edit group_vars/all/vault.yml`
2. albo skopiuj `group_vars/all/vault.yml.example` do `group_vars/all/vault.yml` i zaszyfruj plik poleceniem `ansible-vault encrypt group_vars/all/vault.yml`

Playbook uruchamiaj z hasłem vault, np.:

- `ansible-playbook -i inventory/hosts.yml site.yml --limit rpi-servicebox --ask-vault-pass`
- albo `ansible-playbook -i inventory/hosts.yml site.yml --limit rpi-servicebox --vault-password-file .vault_pass`

## Przykład uruchomienia

Z katalogu `ansible/`:

`ansible-playbook -i inventory/hosts.yml site.yml --limit rpi-servicebox`

Jeśli logujesz się do Raspberry hasłem przez SSH i używasz `sudo`, uruchom:

`ansible-playbook -i inventory/hosts.yml site.yml --limit rpi-servicebox --ask-pass --ask-become-pass`

## Co wdraża playbook

- aktualizuje system pakietów `apt`
- instaluje `git`, `python3`, `python3-venv`, `python3-pip`, `mosquitto`, `mosquitto-clients`
- konfiguruje Mosquitto do nasłuchu na `0.0.0.0:1883`
- klonuje repo do `/opt/ar-iot-servicebox`
- tworzy `server/.venv`
- instaluje zależności z `server/requirements.txt`
- instaluje usługę `ar-iot-server.service`
- uruchamia usługę i włącza autostart
- gdy `node_red_enabled=true`:
  - instaluje `Node-RED` oficjalnym instalatorem dla systemów Debian-based
  - konfiguruje usługę `nodered` pod użytkownika z `ansible_user`
  - instaluje pakiet `@flowfuse/node-red-dashboard`
  - kopiuje przykładowy flow do `~/.node-red/examples/ar-iot-servicebox-demo-flow.json`

## Weryfikacja na Raspberry Pi

Po wdrożeniu sprawdź:

- `systemctl status mosquitto`
- `systemctl status ar-iot-server`
- `systemctl status nodered`
- `ss -ltnp | grep 1883`

Jeśli włączony jest `Node-RED`, edytor będzie dostępny pod:

- `http://<RPI_IP>:1880`

Po imporcie przykładowego flow dashboard będzie dostępny pod:

- `http://<RPI_IP>:1880/dashboard/ar-iot`
