from __future__ import annotations

import pytest

from ar_iot_server.quest_bridge import translate_quest_command, translate_telemetry


def test_translate_telemetry_maps_selected_fan_and_pwm() -> None:
    result = translate_telemetry(
        {
            "device_id": "ARBOX-001",
            "timestamp_ms": 3210,
            "fan_select": True,
            "fan_a_rpm": 1234,
            "fan_b_rpm": 999,
            "fan_pwm": 128,
            "temperature_env_c": 24.5,
            "humidity_pct": 41.2,
            "system_state": "WARNING",
            "error_code": "OVERHEAT",
        }
    )

    assert result.telemetry_topic_device_id == "ARBOX-001"
    assert result.telemetry_payload["device_id"] == "ARBOX-001"
    assert result.telemetry_payload["fanSelected"] == "A"
    assert result.telemetry_payload["fanReportedRPM"] == 1234
    assert result.telemetry_payload["fanReportedPWM"] == 50
    assert result.telemetry_payload["systemState"] == "WARNING"


def test_translate_quest_command_rejects_direct_fan_selection() -> None:
    with pytest.raises(ValueError, match="Only fan='selected' is currently supported"):
        translate_quest_command({"command": "set_fan_pwm", "fan": "A", "value": 80})
