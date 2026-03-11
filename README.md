# City-Mapper 🗺️

**Production-grade Paris Metro route planner** — models the RATP transit network as a weighted directed graph and computes optimal paths using Dijkstra's algorithm and A*.

## Architecture

| Layer | Technology | Description |
|-------|-----------|-------------|
| **Frontend** | Angular 19 + Material 3 + Leaflet | Interactive map with route visualization |
| **Backend** | C++20 + Drogon | REST API with optimized pathfinding engine |
| **Data** | RATP CSV/GTFS | 760 stations, 2428 connections |
| **Infra** | Docker + Traefik | Containerized deployment with reverse proxy |

## Prerequisites

| Tool | Version | Notes |
|------|---------|-------|
| C++20 compiler | GCC 13+ / Clang 16+ / Apple Clang 15+ | |
| CMake | ≥ 3.20 | |
| Conan | ≥ 2.0 | `pip install conan` |
| Node.js | ≥ 18 | For the Angular frontend |
| Angular CLI | ≥ 19 | `npm install -g @angular/cli` |
| Docker *(optional)* | ≥ 24 | Only for containerized deployment |

## Quick Start

### Backend (C++ API)

```bash
# 1. Install dependencies via Conan
cd backend
conan install . --output-folder=build --build=missing \
    -s build_type=Release -s compiler.cppstd=20

# 2. Build with CMake
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build . --parallel

# 3. Run the server (works from any directory!)
./city_mapper_server
```

The server **auto-detects data file locations** — you can run the binary from `backend/build/`, the project root, or anywhere else. It resolves paths automatically.

The API server starts on **http://localhost:8080**.

### Frontend (Angular)

```bash
cd frontend
npm install
npm start
```

The frontend dev server starts on **http://localhost:4200** and proxies API calls to `localhost:8080`.

### Docker (Full Stack)

```bash
docker compose up --build
```

| Service | URL |
|---------|-----|
| Frontend | http://citymapper.localhost |
| Backend API | http://api.citymapper.localhost |
| Traefik Dashboard | http://localhost:8090 |

## Configuration

The backend reads configuration from **environment variables**. When no env vars are set, it auto-resolves paths from the compiled binary's location.

| Variable | Default | Description |
|----------|---------|-------------|
| `DATA_PATH` | *auto-resolved* `data/` | Directory containing station & connection CSVs |
| `STATIONS_FILE` | `s.csv` | Stations CSV filename |
| `CONNECTIONS_FILE` | `c.csv` | Connections CSV filename |
| `GTFS_PATH` | *auto-resolved* `data/IDFM-gtfs/` | Directory containing GTFS data |
| `GTFS_STOPS_FILE` | `stops.txt` | GTFS stops filename |
| `HOST` | `0.0.0.0` | Server bind address |
| `PORT` | `8080` | Server listen port |
| `CACHE_SIZE` | `10000` | Route cache capacity (LRU) |
| `THREAD_COUNT` | `4` | Drogon I/O thread count |

**Path resolution order** (when `DATA_PATH` / `GTFS_PATH` are not set):

1. Compile-time project root (embedded by CMake at build time)
2. Walk up from executable location to find `data/csv/s.csv`
3. Walk up from current working directory (last resort)

Example — override via env vars:
```bash
DATA_PATH=/custom/data GTFS_PATH=/custom/gtfs ./city_mapper_server
```

## API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| `GET` | `/health` | Service health + graph stats |
| `GET` | `/api/stations` | List all stations |
| `GET` | `/api/stations/{id}` | Get station by ID |
| `GET` | `/api/stations/search?q=chatelet` | Search stations by name |
| `POST` | `/api/route` | Compute shortest route |

### Example: Compute Route

```bash
curl -X POST http://localhost:8080/api/route \
  -H "Content-Type: application/json" \
  -d '{"from_id": 1642, "to_id": 1648, "algorithm": "a_star"}'
```

## Algorithms

| Algorithm | Complexity | Use Case |
|-----------|-----------|----------|
| **Dijkstra** | O((V+E) log V) | Guaranteed shortest path |
| **A*** | O((V+E) log V) | Faster via Haversine heuristic |

Both use `std::priority_queue` (min-heap).

## Project Structure

```
City-Mapper/
├── backend/                    # C++20 Drogon REST API
│   ├── src/
│   │   ├── core/
│   │   │   ├── algorithms/     # Dijkstra, A* (Strategy pattern)
│   │   │   ├── cache/          # Thread-safe LRU cache
│   │   │   ├── graph/          # Station, Connection, Graph models
│   │   │   └── utils/          # Haversine, TimeFormatter, StringUtils
│   │   ├── data/
│   │   │   ├── parsers/        # CSV & GTFS parsers (Strategy pattern)
│   │   │   └── loaders/        # GraphLoader orchestrator
│   │   ├── api/
│   │   │   ├── controllers/    # Health, Station, Route endpoints
│   │   │   └── middleware/     # CORS filter
│   │   └── config/             # Environment-based configuration
│   └── tests/                  # Google Test unit tests
├── frontend/                   # Angular 19 + Material 3 + Leaflet
├── data/                       # RATP data files
│   ├── csv/                    # s.csv (stations), c.csv (connections)
│   └── IDFM-gtfs/             # GTFS stops for geo-coordinates
├── docs/                       # Architecture, API, Algorithm docs
└── docker-compose.yml          # Full-stack orchestration
```

## Running Tests

```bash
cd backend/build
ctest --output-on-failure
```

## Documentation

- [Architecture Overview](docs/ARCHITECTURE.md)
- [API Reference](docs/API_SCHEMA.md)
- [Algorithm Details](docs/ALGORITHMS.md)