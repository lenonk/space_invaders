#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $(basename "$0") file1.svg [file2.svg ...]" >&2
  exit 1
fi

convert_one() {
  local svg="$1"

  # Read width/height from attributes; fallback to viewBox if missing
  local width height
  width=$(xmlstarlet sel -t -v "/*[local-name()='svg']/@width"  "$svg" || true)
  height=$(xmlstarlet sel -t -v "/*[local-name()='svg']/@height" "$svg" || true)

  if [[ -z "${width}" || -z "${height}" ]]; then
    # viewBox: minx miny w h
    read -r _ _ vw vh < <(xmlstarlet sel -t -v "/*[local-name()='svg']/@viewBox" "$svg")
    width="${width:-$vw}"
    height="${height:-$vh}"
  fi

  # Strip units (px) and decimals for integer export sizes
  width=${width%px};   height=${height%px}
  width=${width%.*};   height=${height%.*}

  if [[ -z "${width}" || -z "${height}" ]]; then
    echo "Failed to read size from: $svg" >&2
    return 1
  fi

  echo "SVG size (${svg}): ${width}x${height}"

  filename=$(basename "$svg" .svg)
  ofile="$(dirname "$svg")/../${filename}.png"
  rsvg-convert -w "${width}" -h "${height}" "$svg" -o "$ofile"
}

for svg in "$@"; do
  convert_one "$svg"
done
