from __future__ import annotations

import json
from dataclasses import dataclass
from typing import Any, Callable

import paho.mqtt.client as mqtt

from .config import Settings


TelemetryHandler = Callable[[str, dict[str, Any]], None]
BridgeTelemetryHandler = Callable[[str, dict[str, Any], mqtt.Client], None]
QuestCommandHandler = Callable[[str, dict[str, Any], mqtt.Client], None]


@dataclass
class MqttRuntime:
    settings: Settings

    def create_client(self, client_id: str) -> mqtt.Client:
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=client_id)
        if self.settings.mqtt_username:
            client.username_pw_set(self.settings.mqtt_username, self.settings.mqtt_password)
        return client

    def connect(self, client: mqtt.Client) -> None:
        client.connect(self.settings.mqtt_host, self.settings.mqtt_port, keepalive=60)

    def publish_command(self, device_id: str, payload: dict[str, Any]) -> None:
        client = self.create_client("ar-iot-cli-publisher")
        self.connect(client)
        topic = self.settings.command_topic_template.format(device_id=device_id)
        result = client.publish(topic, json.dumps(payload, ensure_ascii=True), qos=1, retain=False)
        result.wait_for_publish()
        client.disconnect()

    def publish_quest_command(self, device_id: str, payload: dict[str, Any]) -> None:
        client = self.create_client("ar-iot-cli-quest-publisher")
        self.connect(client)
        topic = self.settings.quest_command_topic_template.format(device_id=device_id)
        result = client.publish(topic, json.dumps(payload, ensure_ascii=True), qos=1, retain=False)
        result.wait_for_publish()
        client.disconnect()

    def publish_json(self, client: mqtt.Client, topic: str, payload: dict[str, Any]) -> None:
        client.publish(topic, json.dumps(payload, ensure_ascii=True), qos=1, retain=False)

    def run_telemetry_monitor(self, on_telemetry: TelemetryHandler) -> None:
        client = self.create_client("ar-iot-server-monitor")

        def handle_connect(
            mqtt_client: mqtt.Client,
            _: Any,
            __: Any,
            reason_code: Any,
            ___: Any,
        ) -> None:
            if getattr(reason_code, "value", reason_code) != 0:
                raise RuntimeError(f"MQTT connect failed with code {reason_code}")
            mqtt_client.subscribe(self.settings.telemetry_topic, qos=1)

        def handle_message(_: mqtt.Client, __: Any, message: mqtt.MQTTMessage) -> None:
            try:
                payload = json.loads(message.payload.decode("utf-8"))
            except json.JSONDecodeError:
                return

            on_telemetry(message.topic, payload)

        client.on_connect = handle_connect
        client.on_message = handle_message
        self.connect(client)
        client.loop_forever()

    def run_bridge(
        self,
        on_telemetry: BridgeTelemetryHandler,
        on_quest_command: QuestCommandHandler,
    ) -> None:
        client = self.create_client("ar-iot-server-bridge")

        def handle_connect(
            mqtt_client: mqtt.Client,
            _: Any,
            __: Any,
            reason_code: Any,
            ___: Any,
        ) -> None:
            if getattr(reason_code, "value", reason_code) != 0:
                raise RuntimeError(f"MQTT connect failed with code {reason_code}")
            mqtt_client.subscribe(self.settings.telemetry_topic, qos=1)
            mqtt_client.subscribe(self.settings.quest_command_topic, qos=1)

        def handle_message(mqtt_client: mqtt.Client, __: Any, message: mqtt.MQTTMessage) -> None:
            try:
                payload = json.loads(message.payload.decode("utf-8"))
            except json.JSONDecodeError:
                return

            if message.topic.endswith("/telemetry") and "/quest/" not in message.topic:
                on_telemetry(message.topic, payload, mqtt_client)
                return

            if message.topic.endswith("/quest/command"):
                on_quest_command(message.topic, payload, mqtt_client)

        client.on_connect = handle_connect
        client.on_message = handle_message
        self.connect(client)
        client.loop_forever()
