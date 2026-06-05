#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CAST="${1:-$ROOT/docs/as5600.cast}"
SERIAL="${SERIAL:-/dev/ttyACM0}"
DURATION="${DURATION:-12}"

mkdir -p "$(dirname "$CAST")"

cd "$ROOT/tools"
uv run asciinema rec \
    -c "stty -F '$SERIAL' 115200 raw -echo && timeout '$DURATION' cat '$SERIAL'" \
    -t "AS5600 hall sensor test on RP2350" \
    --overwrite \
    "$CAST"

echo "Saved: $CAST"
echo "Play:  uv run --project tools asciinema cat '$CAST'"
echo "Upload: uv run --project tools asciinema upload '$CAST'"
