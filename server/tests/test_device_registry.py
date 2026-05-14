from __future__ import annotations

from pathlib import Path

from ar_iot_server.device_registry import list_devices_from_log
from ar_iot_server.logging_utils import append_json_line


def test_list_devices_from_log_returns_latest_per_device(tmp_path: Path) -> None:
    log_path = tmp_path / "telemetry.jsonl"

    append_json_line(
        log_path,
        {"device_id": "servicebox-lite-001", "system_state": "OK", "timestamp_ms": 1000, "error_code": ""},
    )
    append_json_line(
        log_path,
        {
            "device_id": "servicebox-lite-001",
            "system_state": "ERROR",
            "timestamp_ms": 2000,
            "error_code": "FAN_BLOCKED",
        },
    )
    append_json_line(
        log_path,
        {"device_id": "servicebox-lite-002", "system_state": "WARNING", "timestamp_ms": 1500, "error_code": ""},
    )

    devices = list_devices_from_log(log_path)

    assert [device.device_id for device in devices] == ["servicebox-lite-001", "servicebox-lite-002"]
    assert devices[0].system_state == "ERROR"
    assert devices[0].timestamp_ms == 2000
    assert devices[0].error_code == "FAN_BLOCKED"
