from __future__ import annotations

import os
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Settings:
    mqtt_host: str = os.getenv("AR_IOT_MQTT_HOST", "127.0.0.1")
    mqtt_port: int = int(os.getenv("AR_IOT_MQTT_PORT", "1883"))
    mqtt_username: str = os.getenv("AR_IOT_MQTT_USERNAME", "")
    mqtt_password: str = os.getenv("AR_IOT_MQTT_PASSWORD", "")
    telemetry_topic: str = os.getenv("AR_IOT_TELEMETRY_TOPIC", "arbox/+/telemetry")
    command_topic_template: str = os.getenv("AR_IOT_COMMAND_TOPIC_TEMPLATE", "arbox/{device_id}/command")
    quest_telemetry_topic_template: str = os.getenv(
        "AR_IOT_QUEST_TELEMETRY_TOPIC_TEMPLATE",
        "arbox/{device_id}/quest/telemetry",
    )
    quest_command_topic: str = os.getenv("AR_IOT_QUEST_COMMAND_TOPIC", "arbox/+/quest/command")
    quest_command_topic_template: str = os.getenv(
        "AR_IOT_QUEST_COMMAND_TOPIC_TEMPLATE",
        "arbox/{device_id}/quest/command",
    )
    log_path: Path = Path(os.getenv("AR_IOT_LOG_PATH", "logs/telemetry.jsonl"))


def get_settings() -> Settings:
    return Settings()
