from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class DeviceSummary:
    device_id: str
    system_state: str
    timestamp_ms: int
    error_code: str


def list_devices_from_log(log_path: Path) -> list[DeviceSummary]:
    if not log_path.exists():
        return []

    latest_by_device: dict[str, DeviceSummary] = {}
    with log_path.open("r", encoding="utf-8") as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue

            try:
                payload = json.loads(line)
            except json.JSONDecodeError:
                continue

            device_id = str(payload.get("device_id", "")).strip()
            if not device_id:
                continue

            latest_by_device[device_id] = DeviceSummary(
                device_id=device_id,
                system_state=str(payload.get("system_state", "UNKNOWN")),
                timestamp_ms=int(payload.get("timestamp_ms", 0) or 0),
                error_code=str(payload.get("error_code", "")),
            )

    return sorted(latest_by_device.values(), key=lambda item: item.device_id)
