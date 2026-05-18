from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class QuestBridgeResult:
    device_id: str
    telemetry_payload: dict[str, Any]
    telemetry_topic_device_id: str


def translate_telemetry(payload: dict[str, Any]) -> QuestBridgeResult:
    source_device_id = str(payload.get("device_id", "")).strip()
    telemetry_topic_device_id = source_device_id or "unknown-device"

    fan_selected = "A" if bool(payload.get("fan_select", False)) else "B"
    fan_reported_rpm = int(payload.get("fan_a_rpm", 0) or 0) if fan_selected == "A" else int(
        payload.get("fan_b_rpm", 0) or 0
    )
    raw_pwm = int(payload.get("fan_pwm", 0) or 0)
    pwm_percent = round((raw_pwm / 255.0) * 100)

    translated = {
        "device_id": telemetry_topic_device_id,
        "timestamp": int(payload.get("timestamp_ms", 0) or 0),
        "fanReportedRPM": fan_reported_rpm,
        "fanReportedPWM": pwm_percent,
        "fanSelected": fan_selected,
        "sensorTemperature": float(payload.get("temperature_env_c", 0) or 0),
        "sensorHumidity": float(payload.get("humidity_pct", 0) or 0),
        "systemState": str(payload.get("system_state", "UNKNOWN")),
        "errorCode": str(payload.get("error_code", "")),
    }

    return QuestBridgeResult(
        device_id=translated["device_id"],
        telemetry_payload=translated,
        telemetry_topic_device_id=telemetry_topic_device_id,
    )


def translate_quest_command(payload: dict[str, Any]) -> dict[str, Any]:
    command = str(payload.get("command", "")).strip()

    if command == "set_fan_pwm":
        fan = str(payload.get("fan", "selected")).strip()
        if fan != "selected":
            raise ValueError("Only fan='selected' is currently supported")

        return {
            "command": "set_fan_pwm",
            "value": int(payload.get("value", 0) or 0),
        }

    if command == "simulate_fault":
        return {
            "command": "simulate_fault",
            "fault": str(payload.get("fault", "")).strip(),
        }

    if command == "resume_auto_pwm":
        return {"command": "resume_auto_pwm"}

    raise ValueError(f"Unsupported Quest command: {command}")
