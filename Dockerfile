# Ubuntu 24.04 base image for building firebolt-native
# Supports both eyil protocol and websocket transport implementations
FROM ubuntu:24.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Build arguments to control which implementations to build
ARG BUILD_WEBSOCKET=ON
ARG BUILD_EYIL=ON

# Install common build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    g++ \
    git \
    pkg-config \
    wget \
    libssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Protobuf (from apt for eyil, may need newer for websocket)
RUN apt-get update && apt-get install -y \
    protobuf-compiler \
    libprotobuf-dev \
    && rm -rf /var/lib/apt/lists/*

# Install ZeroMQ (for eyil implementation)
RUN apt-get update && apt-get install -y \
    libzmq3-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Google Test
RUN apt-get update && apt-get install -y \
    libgtest-dev \
    && cd /usr/src/gtest && \
    cmake CMakeLists.txt && \
    make && \
    cp lib/*.a /usr/lib && \
    cd / && \
    apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install WebSocket++ (header-only library, for websocket transport)
RUN wget https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.tar.gz && \
    tar xzf 0.8.2.tar.gz && \
    mkdir -p /usr/local/include/websocketpp && \
    cp -r websocketpp-0.8.2/websocketpp/* /usr/local/include/websocketpp/ && \
    rm -rf websocketpp-0.8.2 0.8.2.tar.gz

# Install standalone Asio (header-only library, compatible with WebSocket++ 0.8.2)
RUN wget https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-12-2.tar.gz && \
    tar xzf asio-1-12-2.tar.gz && \
    mkdir -p /usr/local/include/asio && \
    cp -r asio-asio-1-12-2/asio/include/asio/* /usr/local/include/asio/ && \
    cp asio-asio-1-12-2/asio/include/asio.hpp /usr/local/include/ && \
    rm -rf asio-asio-1-12-2 asio-1-12-2.tar.gz

# Add /usr/local to CMAKE paths for locally installed libraries
ENV CMAKE_PREFIX_PATH="/usr/local"
ENV CMAKE_LIBRARY_PATH="/usr/local/lib"
ENV CMAKE_INCLUDE_PATH="/usr/local/include"

# Set working directory
WORKDIR /workspace
