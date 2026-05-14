from __future__ import annotations

import os
from dataclasses import dataclass, field
from pathlib import Path


def _load_dotenv(dotenv_path: Path) -> None:
    if not dotenv_path.exists():
        return

    for raw_line in dotenv_path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue

        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip().strip("\"'")
        os.environ.setdefault(key, value)


@dataclass(frozen=True)
class Settings:
    mqtt_host: str = field(default_factory=lambda: os.getenv("AR_IOT_MQTT_HOST", "127.0.0.1"))
    mqtt_port: int = field(default_factory=lambda: int(os.getenv("AR_IOT_MQTT_PORT", "1883")))
    mqtt_username: str = field(default_factory=lambda: os.getenv("AR_IOT_MQTT_USERNAME", ""))
    mqtt_password: str = field(default_factory=lambda: os.getenv("AR_IOT_MQTT_PASSWORD", ""))
    telemetry_topic: str = field(default_factory=lambda: os.getenv("AR_IOT_TELEMETRY_TOPIC", "arbox/+/telemetry"))
    command_topic_template: str = field(
        default_factory=lambda: os.getenv("AR_IOT_COMMAND_TOPIC_TEMPLATE", "arbox/{device_id}/command")
    )
    quest_telemetry_topic_template: str = field(
        default_factory=lambda: os.getenv(
            "AR_IOT_QUEST_TELEMETRY_TOPIC_TEMPLATE",
            "arbox/{device_id}/quest/telemetry",
        )
    )
    quest_command_topic: str = field(
        default_factory=lambda: os.getenv("AR_IOT_QUEST_COMMAND_TOPIC", "arbox/+/quest/command")
    )
    quest_command_topic_template: str = field(
        default_factory=lambda: os.getenv(
            "AR_IOT_QUEST_COMMAND_TOPIC_TEMPLATE",
            "arbox/{device_id}/quest/command",
        )
    )
    log_path: Path = field(default_factory=lambda: Path(os.getenv("AR_IOT_LOG_PATH", "logs/telemetry.jsonl")))


def get_settings() -> Settings:
    _load_dotenv(Path(".env"))
    return Settings()
