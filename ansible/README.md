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
- `ar_iot_server_mqtt_username`
- `ar_iot_server_mqtt_password`

## Przykład uruchomienia

Z katalogu `ansible/`:

`ansible-playbook -i inventory/hosts.yml site.yml --limit rpi-servicebox`

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
