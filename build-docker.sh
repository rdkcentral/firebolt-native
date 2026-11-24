#!/bin/bash
# Two-stage build script for firebolt-native
# First builds and installs eyil-secure-gateway, then builds firebolt-native

set -e  # Exit on error

# Configuration
EYIL_GATEWAY_DIR="${EYIL_GATEWAY_DIR:-/workspace/eyil-secure-gateway}"
FIREBOLT_NATIVE_DIR="${FIREBOLT_NATIVE_DIR:-/workspace/firebolt-native}"
BUILD_DIR="${BUILD_DIR:-/workspace/build}"

echo "=== Stage 1: Building eyil-secure-gateway ==="
echo "EYIL_GATEWAY_DIR: ${EYIL_GATEWAY_DIR}"

# Build eyil-secure-gateway
mkdir -p "${BUILD_DIR}/eyil-gateway"
cd "${BUILD_DIR}/eyil-gateway"

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DPLUGIN_EYILGATEWAY=OFF \
    -DBUILD_EYILCLIENT=ON \
    "${EYIL_GATEWAY_DIR}"

ninja

# Install eyil-secure-gateway to /usr
echo "Installing eyil-secure-gateway to /usr..."
ninja install

echo "=== Stage 2: Building firebolt-native ==="
echo "FIREBOLT_NATIVE_DIR: ${FIREBOLT_NATIVE_DIR}"

# Build firebolt-native
mkdir -p "${BUILD_DIR}/firebolt-native"
cd "${BUILD_DIR}/firebolt-native"

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    "${FIREBOLT_NATIVE_DIR}"

ninja

echo "=== Build complete ==="
echo "eyil-secure-gateway installed to /usr"
echo "firebolt-native built in ${BUILD_DIR}/firebolt-native"
