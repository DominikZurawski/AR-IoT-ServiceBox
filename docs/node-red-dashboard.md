# Node-RED Dashboard

## Cel

`Node-RED` w tym projekcie jest opcjonalną warstwą operatorską na `Raspberry Pi`.

Nie zastępuje bridge'a Python.

Służy do:

- podglądu telemetrii MQTT
- ręcznego wysyłania komend testowych
- prostego dashboardu demonstracyjnego z przeglądarki

## Co wdraża Ansible

Jeśli w `inventory/hosts.yml` ustawisz:

```yaml
node_red_enabled: true
```

playbook:

- instaluje `Node-RED` oficjalnym instalatorem dla systemów Debian-based
- ustawia usługę `nodered` pod użytkownika z `ansible_user`
- instaluje `@flowfuse/node-red-dashboard`
- kopiuje przykładowy flow do:
  - `~/.node-red/examples/ar-iot-servicebox-demo-flow.json`

## Dostęp

Po wdrożeniu:

- edytor `Node-RED`: `http://<RPI_IP>:1880`
- dashboard po imporcie flow: `http://<RPI_IP>:1880/dashboard/ar-iot`

## Import przykładowego flow

1. Otwórz edytor `Node-RED`
2. wybierz `Import`
3. wczytaj plik:
   - `~/.node-red/examples/ar-iot-servicebox-demo-flow.json`
4. kliknij `Deploy`

## Co zawiera flow

- subskrypcję `arbox/+/telemetry`
- subskrypcję `arbox/+/quest/telemetry`
- panel komend publikujący na `arbox/<DEVICE_ID>/quest/command`
- podgląd debug w sidebarze `Node-RED`
- prosty dashboard z kartami telemetrii

## Domyślne testy

Domyślny `device_id` w panelu komend:

`ARBOX-001`

Możesz go zmienić bezpośrednio w dashboardzie przed wysłaniem komendy.

## Typowy scenariusz

1. `ESP32` lub symulator publikuje techniczną telemetrię na `arbox/<DEVICE_ID>/telemetry`
2. serwer Python tłumaczy dane do `arbox/<DEVICE_ID>/quest/telemetry`
3. `Node-RED` pokazuje oba widoki równolegle
4. z dashboardu wysyłasz komendę na `arbox/<DEVICE_ID>/quest/command`
5. bridge Python tłumaczy ją dalej do `arbox/<DEVICE_ID>/command`
