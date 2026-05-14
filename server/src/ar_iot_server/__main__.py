from __future__ import annotations

import sys

from .cli import build_parser, run_cli
from .config import get_settings


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    settings = get_settings()
    return run_cli(args, settings)


if __name__ == "__main__":
    raise SystemExit(main())
