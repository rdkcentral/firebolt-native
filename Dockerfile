FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    wget \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Install newer Protobuf from source (3.21+ for protobuf_generate_cpp support)
WORKDIR /tmp
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-cpp-3.21.12.tar.gz && \
    tar xzf protobuf-cpp-3.21.12.tar.gz && \
    cd protobuf-3.21.12 && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    cd /tmp && \
    rm -rf protobuf-3.21.12 protobuf-cpp-3.21.12.tar.gz

# Install WebSocket++ (header-only library)
WORKDIR /tmp
RUN wget https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.tar.gz && \
    tar xzf 0.8.2.tar.gz && \
    mkdir -p /usr/local/include/websocketpp && \
    cp -r websocketpp-0.8.2/websocketpp/* /usr/local/include/websocketpp/ && \
    rm -rf websocketpp-0.8.2 0.8.2.tar.gz

# Install standalone Asio (header-only library) - use version compatible with WebSocket++ 0.8.2
RUN wget https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-12-2.tar.gz && \
    tar xzf asio-1-12-2.tar.gz && \
    mkdir -p /usr/local/include/asio && \
    cp -r asio-asio-1-12-2/asio/include/asio/* /usr/local/include/asio/ && \
    cp asio-asio-1-12-2/asio/include/asio.hpp /usr/local/include/ && \
    rm -rf asio-asio-1-12-2 asio-1-12-2.tar.gz

# Set working directory for the build
WORKDIR /project

# Copy project files
COPY . /project

# Build the project
RUN mkdir build && \
    cd build && \
    cmake .. -DBUILD_TESTING=ON && \
    make firebolt-core-transport test_websocket_transport mock_websocket_server && \
    ./transports/websocket/test_websocket_transport

# Default command shows the built executables
CMD ls -la build/transports/websocket/ && echo "Mock server available at: build/mock_websocket_server"
