from __future__ import annotations

from pathlib import Path

from ar_iot_server.config import get_settings


def test_get_settings_loads_dotenv(tmp_path: Path, monkeypatch) -> None:
    monkeypatch.chdir(tmp_path)
    monkeypatch.delenv("AR_IOT_MQTT_HOST", raising=False)
    monkeypatch.delenv("AR_IOT_MQTT_PORT", raising=False)
    monkeypatch.delenv("AR_IOT_LOG_PATH", raising=False)

    (tmp_path / ".env").write_text(
        "AR_IOT_MQTT_HOST=192.168.50.20\n"
        "AR_IOT_MQTT_PORT=2883\n"
        "AR_IOT_LOG_PATH=runtime/telemetry.jsonl\n",
        encoding="utf-8",
    )

    settings = get_settings()

    assert settings.mqtt_host == "192.168.50.20"
    assert settings.mqtt_port == 2883
    assert settings.log_path == Path("runtime/telemetry.jsonl")
