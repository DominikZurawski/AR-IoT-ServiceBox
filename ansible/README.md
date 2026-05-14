# Ansible

Automatyzacja przygotowania Raspberry Pi i wdrożenia serwera.

## Zakres

- rola `raspberry_pi_base` instaluje bazowe pakiety i uruchamia Mosquitto
- rola `raspberry_pi_base` wystawia Mosquitto na porcie `1883` w LAN
- rola `ar_iot_server` klonuje repozytorium, tworzy `venv`, instaluje zależności i uruchamia `python -m ar_iot_server monitor` jako usługę `systemd`

## Wymagane zmienne

W `inventory/hosts.yml` ustaw co najmniej:

- `ansible_host`
- `ansible_user`
- `ar_iot_server_repo_url`
- opcjonalnie `ar_iot_server_repo_version`

Jeśli broker lub serwer mają używać innych parametrów niż domyślne, ustaw też:

- `ar_iot_server_mqtt_host`
- `ar_iot_server_mqtt_port`

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

`ansible-playbook -i inventory/hosts.yml site.yml --limit rpi-servicebox --ask-vault-pass`

## Co wdraża playbook

- aktualizuje system pakietów `apt`
- instaluje `git`, `python3`, `python3-venv`, `python3-pip`, `mosquitto`, `mosquitto-clients`
- konfiguruje Mosquitto do nasłuchu na `0.0.0.0:1883`
- klonuje repo do `/opt/ar-iot-servicebox`
- tworzy `server/.venv`
- instaluje zależności z `server/requirements.txt`
- instaluje usługę `ar-iot-server.service`
- uruchamia usługę i włącza autostart

## Weryfikacja na Raspberry Pi

Po wdrożeniu sprawdź:

- `systemctl status mosquitto`
- `systemctl status ar-iot-server`
- `ss -ltnp | grep 1883`
