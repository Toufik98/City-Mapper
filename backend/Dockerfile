# ============================================================
# City-Mapper Backend — Multi-stage Docker build
# ============================================================
# Stage 1: Build (gcc + cmake + conan)
# Stage 2: Runtime (minimal image with just the binary + data)
# ============================================================

# ── Stage 1: Build ──────────────────────────────────────────
FROM gcc:13 AS builder

# Install build tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Install Conan
RUN pip3 install --break-system-packages conan && \
    conan profile detect

WORKDIR /build

# Copy Conan file first (for layer caching)
COPY conanfile.txt .

# Install dependencies
RUN conan install . --output-folder=build --build=missing \
    -s build_type=Release \
    -s compiler.cppstd=20

# Copy source code
COPY CMakeLists.txt .
COPY src/ src/
COPY tests/ tests/

# Build
RUN cd build && \
    cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DBUILD_TESTS=OFF && \
    cmake --build . --parallel $(nproc)

# ── Stage 2: Runtime ───────────────────────────────────────
FROM debian:bookworm-slim AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN groupadd -r citymap && useradd -r -g citymap citymap

WORKDIR /app

# Copy binary
COPY --from=builder /build/build/city_mapper_server .

# Copy data files
# Copy data files from the build context (project root)
COPY data/ ./data/
COPY data/IDFM-gtfs/ ./data/IDFM-gtfs/

# Set ownership
RUN chown -R citymap:citymap /app

USER citymap

# Environment defaults
ENV DATA_PATH=/app/data
ENV STATIONS_FILE=s.csv
ENV CONNECTIONS_FILE=c.csv
ENV GTFS_PATH=/app/data/IDFM-gtfs
ENV GTFS_STOPS_FILE=stops.txt
ENV HOST=0.0.0.0
ENV PORT=8080
ENV CACHE_SIZE=10000
ENV THREAD_COUNT=4

EXPOSE 8080

HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

ENTRYPOINT ["./city_mapper_server"]
