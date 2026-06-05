#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CAST="${1:-$ROOT/docs/as5600.cast}"
SERIAL="${SERIAL:-/dev/ttyACM0}"
DURATION="${DURATION:-12}"
TITLE="${TITLE:-AS5600 hall sensor test on RP2350}"
UPLOAD="${UPLOAD:-0}"

usage() {
    cat <<EOF
Usage: $(basename "$0") [cast-file]

Record AS5600 USB CDC output with asciinema.

Environment:
  SERIAL=/dev/ttyACM0   serial device
  DURATION=12           capture seconds
  TITLE=...             recording title
  UPLOAD=1              upload after recording
  GIF=1                 also render docs/demo.gif (requires agg)

Examples:
  $(basename "$0")
  UPLOAD=1 $(basename "$0")
  SERIAL=/dev/ttyACM1 DURATION=15 $(basename "$0") docs/demo.cast
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
    usage
    exit 0
fi

mkdir -p "$(dirname "$CAST")"

asciinema rec \
    -c "stty -F '$SERIAL' 115200 raw -echo && timeout '$DURATION' cat '$SERIAL'" \
    -t "$TITLE" \
    --overwrite \
    "$CAST"

echo "Saved: $CAST"
echo "Play:  asciinema cat '$CAST'"

if [[ "$UPLOAD" == "1" ]]; then
    asciinema upload "$CAST"
else
    echo "Upload: asciinema upload '$CAST'"
fi

if [[ "${GIF:-0}" == "1" ]]; then
    GIF_OUT="${GIF_OUT:-$(dirname "$CAST")/demo.gif}"
    if command -v agg >/dev/null 2>&1; then
        agg "$CAST" "$GIF_OUT"
        echo "GIF:   $GIF_OUT"
    else
        echo "Install agg to build GIF: https://github.com/asciinema/agg"
        echo "Example: agg '$CAST' '$GIF_OUT'"
    fi
fi
