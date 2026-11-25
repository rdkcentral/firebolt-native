#!/bin/bash
# Helper script to build and test firebolt-native with Docker

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_usage() {
    echo "Usage: $0 <command> [options]"
    echo ""
    echo "Commands:"
    echo "  build-ws          Build websocket transport implementation"
    echo "  test-ws           Test websocket transport implementation"
    echo "  build-eyil        Build eyil protocol implementation (requires eyil-secure-gateway)"
    echo "  shell             Open interactive shell in container"
    echo "  help              Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build-ws"
    echo "  $0 test-ws"
    echo "  $0 build-eyil"
    echo "  $0 shell"
}

build_websocket() {
    echo -e "${GREEN}Building websocket transport implementation...${NC}"
    docker build -t firebolt-native-ws .
    docker run --rm \
        -v "$(pwd)":/workspace \
        firebolt-native-ws \
        bash -c "cd /workspace && mkdir -p build && cd build && cmake .. -DBUILD_TESTING=ON && make"
    echo -e "${GREEN}Build complete!${NC}"
}

test_websocket() {
    echo -e "${GREEN}Running websocket transport tests...${NC}"
    if [ ! -d "build" ]; then
        echo -e "${YELLOW}Build directory not found. Building first...${NC}"
        build_websocket
    fi
    docker run --rm \
        -v "$(pwd)":/workspace \
        firebolt-native-ws \
        bash -c "cd /workspace/build && ./transports/websocket/test_websocket_transport"
}

build_eyil() {
    echo -e "${GREEN}Building eyil protocol implementation...${NC}"
    if [ ! -d "../eyil-secure-gateway" ]; then
        echo -e "${RED}Error: eyil-secure-gateway not found at ../eyil-secure-gateway${NC}"
        echo "Please clone eyil-secure-gateway to the parent directory or set EYIL_GATEWAY_DIR"
        exit 1
    fi

    docker build -t firebolt-native-eyil .
    docker run --rm \
        -v "$(pwd)":/workspace/firebolt-native \
        -v "$(pwd)/../eyil-secure-gateway":/workspace/eyil-secure-gateway \
        -v "$(pwd)/build-eyil":/workspace/build \
        -e EYIL_GATEWAY_DIR=/workspace/eyil-secure-gateway \
        firebolt-native-eyil \
        /workspace/firebolt-native/build-docker.sh
    echo -e "${GREEN}Build complete!${NC}"
}

open_shell() {
    echo -e "${GREEN}Opening interactive shell...${NC}"
    docker build -t firebolt-native-shell . > /dev/null 2>&1 || true
    docker run --rm -it \
        -v "$(pwd)":/workspace \
        firebolt-native-shell \
        bash
}

# Parse command
case "${1:-help}" in
    build-ws)
        build_websocket
        ;;
    test-ws)
        test_websocket
        ;;
    build-eyil)
        build_eyil
        ;;
    shell)
        open_shell
        ;;
    help|*)
        print_usage
        ;;
esac
