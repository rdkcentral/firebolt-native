#!/usr/bin/env bash
# Builds the project and runs unit and/or integration tests.
#
# Usage: scripts/run-tests.sh [all|unit|integration] [build-dir]
#   all          Run every registered test (default)
#   unit         Run unit tests only (test_secure_client)
#   integration  Run integration tests only (test_*_lifecycle)
#   build-dir    CMake build directory relative to the repo root (default: build)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

SUITE="${1:-all}"
BUILD_DIR="${REPO_ROOT}/${2:-build}"

case "${SUITE}" in
    all)         FILTER="" ;;
    unit)        FILTER="^test_secure_client$" ;;
    integration) FILTER="^test_.*_lifecycle$" ;;
    *)
        echo "Unknown suite '${SUITE}'. Use all, unit, or integration." >&2
        exit 2
        ;;
esac

JOBS="$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)"

echo "==> Cleaning existing build"
rm -rf "${BUILD_DIR}"

echo "==> Configuring (${BUILD_DIR})"
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}"

echo "==> Building"
cmake --build "${BUILD_DIR}" -j "${JOBS}"

echo "==> Running ${SUITE} tests"
CTEST_ARGS=(--test-dir "${BUILD_DIR}" --output-on-failure)
if [[ -n "${FILTER}" ]]; then
    CTEST_ARGS+=(-R "${FILTER}")
fi
ctest "${CTEST_ARGS[@]}"
