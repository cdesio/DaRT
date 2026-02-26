#!/usr/bin/env bash
set -euo pipefail

# End-to-end decay -> damage -> clustering pipeline.
# This script assumes build directories already exist and binaries are compiled.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DECAY_DIR="${ROOT_DIR}/decay_simulation"
SIM_DIR="${ROOT_DIR}/simulation"
CLUSTER_DIR="${ROOT_DIR}/Clustering"

DECAY_MAC="${DECAY_DIR}/Ra224_quick_test.in"
DECAY_OUT="decay_quick_test"
DECAY_SEED="1"

SIM_MAC="${SIM_DIR}/quick_test_decay.in"
SIM_OUT="damage_from_decay_quick_test"
SIM_SEED="1"

SUGAR_FILE="${ROOT_DIR}/geometryFiles/sugarPos_4x4_300nm.bin"
HISTONE_FILE="${ROOT_DIR}/geometryFiles/histonePositions_4x4_300nm.bin"

if [[ -n "${1:-}" ]]; then
  DECAY_MAC="$1"
fi

if [[ ! -x "${DECAY_DIR}/build/dart" ]]; then
  echo "Missing decay executable: ${DECAY_DIR}/build/dart" >&2
  echo "Build it with: cd ${DECAY_DIR} && mkdir -p build && cd build && cmake .. && make -j" >&2
  exit 1
fi

if [[ ! -x "${SIM_DIR}/build/rbe" ]]; then
  echo "Missing simulation executable: ${SIM_DIR}/build/rbe" >&2
  echo "Build it with: cd ${SIM_DIR} && mkdir -p build && cd build && cmake .. && make -j" >&2
  exit 1
fi

if [[ ! -f "${SUGAR_FILE}" || ! -f "${HISTONE_FILE}" ]]; then
  echo "Missing geometry files." >&2
  echo "Expected:" >&2
  echo "  ${SUGAR_FILE}" >&2
  echo "  ${HISTONE_FILE}" >&2
  exit 1
fi

if ! ls "${CLUSTER_DIR}"/build/clustering*.so >/dev/null 2>&1; then
  echo "Missing clustering module in ${CLUSTER_DIR}/build." >&2
  echo "Build it with: conda activate clustering && cd ${CLUSTER_DIR} && mkdir -p build && cd build && cmake .. && make -j" >&2
  exit 1
fi

CLUSTER_PYTHON_CMD=(python)
if command -v conda >/dev/null 2>&1; then
  if conda run -n clustering python -V >/dev/null 2>&1; then
    CLUSTER_PYTHON_CMD=(conda run -n clustering python)
  fi
fi

echo "[1/3] Running decay simulation"
(
  cd "${DECAY_DIR}/build"
  ./dart -mac "${DECAY_MAC}" -out "${DECAY_OUT}" -seed "${DECAY_SEED}"
)

DECAY_PS="${DECAY_DIR}/build/${DECAY_OUT}.bin"
if [[ ! -f "${DECAY_PS}" ]]; then
  echo "Decay phase-space file not found: ${DECAY_PS}" >&2
  exit 1
fi

echo "[2/3] Running DNA damage simulation with -decayPS"
(
  cd "${SIM_DIR}/build"
  ./rbe -mac "${SIM_MAC}" -out "${SIM_OUT}" -decayPS "${DECAY_PS}" \
    -sugar "${SUGAR_FILE}" -histone "${HISTONE_FILE}" -seed "${SIM_SEED}"
)

SIM_ROOT="${SIM_DIR}/build/${SIM_OUT}.root"
if [[ ! -f "${SIM_ROOT}" ]]; then
  echo "Simulation output not found: ${SIM_ROOT}" >&2
  exit 1
fi

echo "[3/3] Running clustering with simulationType=decay"
(
  cd "${CLUSTER_DIR}"
  export PYTHONPATH="${CLUSTER_DIR}/build:${PYTHONPATH:-}"
  "${CLUSTER_PYTHON_CMD[@]}" run.py --filename "${SIM_ROOT}" --output "${SIM_OUT}.csv" \
    --sugar "${SUGAR_FILE}" --simulationType decay
)

echo "Done."
echo "Decay phase-space: ${DECAY_PS}"
echo "Damage ROOT: ${SIM_ROOT}"
echo "Clustering CSV: ${CLUSTER_DIR}/${SIM_OUT}.csv"
