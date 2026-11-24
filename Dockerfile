# Ubuntu 24.04 base image for building firebolt-native
FROM ubuntu:24.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    g++ \
    protobuf-compiler \
    libprotobuf-dev \
    libzmq3-dev \
    libssl-dev \
    libgtest-dev \
    && cd /usr/src/gtest && \
    cmake CMakeLists.txt && \
    make && \
    cp lib/*.a /usr/lib && \
    cd / && \
    apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace
