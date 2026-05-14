from __future__ import annotations

import argparse
from typing import Any

from .config import Settings
from .device_registry import list_devices_from_log
from .logging_utils import append_json_line
from .mqtt_client import MqttRuntime
from .quest_bridge import translate_quest_command, translate_telemetry


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="AR IoT ServiceBox MQTT server CLI")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("monitor", help="Subscribe to telemetry and log JSON lines")
    subparsers.add_parser("devices", help="List devices discovered in the telemetry log")

    fan_pwm_parser = subparsers.add_parser("set-fan-pwm", help="Send set_fan_pwm command")
    fan_pwm_parser.add_argument("device_id", help="Target device ID")
    fan_pwm_parser.add_argument("value", type=int, help="PWM value 0-100 or 0-255")

    fault_parser = subparsers.add_parser("simulate-fault", help="Send simulate_fault command")
    fault_parser.add_argument("device_id", help="Target device ID")
    fault_parser.add_argument(
        "fault",
        choices=["overtemperature", "fan_blocked", "clear"],
        help="Fault name",
    )

    subparsers.add_parser("resume-auto-pwm", help="Resume automatic PWM sequence").add_argument(
        "device_id",
        help="Target device ID",
    )

    quest_fan_pwm_parser = subparsers.add_parser(
        "quest-set-fan-pwm",
        help="Send Quest-compatible set_fan_pwm command to the bridge topic",
    )
    quest_fan_pwm_parser.add_argument("device_id", help="Target device ID")
    quest_fan_pwm_parser.add_argument("value", type=int, help="PWM percentage 0-100")

    return parser


def run_cli(args: argparse.Namespace, settings: Settings) -> int:
    mqtt_runtime = MqttRuntime(settings)

    if args.command == "monitor":
        return _run_monitor(mqtt_runtime, settings)

    if args.command == "devices":
        return _run_devices(settings)

    if args.command == "set-fan-pwm":
        mqtt_runtime.publish_command(
            args.device_id,
            {"command": "set_fan_pwm", "value": args.value},
        )
        print(f"Sent set_fan_pwm to {args.device_id}: value={args.value}")
        return 0

    if args.command == "simulate-fault":
        mqtt_runtime.publish_command(
            args.device_id,
            {"command": "simulate_fault", "fault": args.fault},
        )
        print(f"Sent simulate_fault to {args.device_id}: fault={args.fault}")
        return 0

    if args.command == "resume-auto-pwm":
        mqtt_runtime.publish_command(args.device_id, {"command": "resume_auto_pwm"})
        print(f"Sent resume_auto_pwm to {args.device_id}")
        return 0

    if args.command == "quest-set-fan-pwm":
        mqtt_runtime.publish_quest_command(
            args.device_id,
            {"command": "set_fan_pwm", "fan": "selected", "value": args.value},
        )
        print(f"Sent Quest set_fan_pwm to {args.device_id}: value={args.value}")
        return 0

    raise ValueError(f"Unsupported command: {args.command}")


def _run_monitor(mqtt_runtime: MqttRuntime, settings: Settings) -> int:
    print(
        f"Connecting to MQTT at {settings.mqtt_host}:{settings.mqtt_port}, "
        f"subscribing to {settings.telemetry_topic} and {settings.quest_command_topic}"
    )

    def handle_telemetry(topic: str, payload: dict[str, Any], monitor_client: Any) -> None:
        append_json_line(settings.log_path, payload)
        print(f"{topic} {payload}")
        quest_payload = translate_telemetry(payload)
        quest_topic = settings.quest_telemetry_topic_template.format(
            device_id=quest_payload.telemetry_topic_device_id
        )
        mqtt_runtime.publish_json(
            monitor_client,
            quest_topic,
            quest_payload.telemetry_payload,
        )

    def handle_quest_command(topic: str, payload: dict[str, Any], monitor_client: Any) -> None:
        try:
            translated = translate_quest_command(payload)
        except ValueError as exc:
            print(f"{topic} rejected quest command: {exc}")
            return

        device_id = _extract_device_id_from_topic(topic)
        device_topic = settings.command_topic_template.format(device_id=device_id)
        mqtt_runtime.publish_json(monitor_client, device_topic, translated)
        print(f"{topic} translated to {device_topic} {translated}")

    mqtt_runtime.run_bridge(handle_telemetry, handle_quest_command)
    return 0


def _run_devices(settings: Settings) -> int:
    devices = list_devices_from_log(settings.log_path)
    if not devices:
        print(f"No devices found in {settings.log_path}")
        return 0

    for device in devices:
        suffix = f" error={device.error_code}" if device.error_code else ""
        print(
            f"{device.device_id} state={device.system_state} "
            f"timestamp_ms={device.timestamp_ms}{suffix}"
        )
    return 0


def _extract_device_id_from_topic(topic: str) -> str:
    parts = topic.split("/")
    if len(parts) < 4:
        raise ValueError(f"Cannot extract device_id from topic: {topic}")
    return parts[1]
