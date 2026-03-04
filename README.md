# City-Mapper 🗺️

**Production-grade Paris Metro route planner** — models the RATP transit network as a weighted directed graph and computes optimal paths using Dijkstra's algorithm and A*.

## Architecture

| Layer | Technology | Description |
|-------|-----------|-------------|
| **Frontend** | Angular 19 + Material 3 + Leaflet | Interactive map with route visualization |
| **Backend** | C++20 + Drogon | REST API with optimized pathfinding engine |
| **Data** | RATP CSV/GTFS | 760 stations, 2428 connections |
| **Infra** | Docker + Traefik | Containerized deployment with reverse proxy |

## Quick Start

### Backend (C++ API)

```bash
cd backend
conan install . --output-folder=build --build=missing -s build_type=Release -s compiler.cppstd=20
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build . --parallel
./city_mapper_server
```

The API server starts on `http://localhost:8080`.

### Docker (Full Stack)

```bash
docker compose up --build
```

- Frontend: `http://citymapper.localhost`
- Backend API: `http://api.citymapper.localhost`
- Traefik dashboard: `http://localhost:8090`

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

Both use `std::priority_queue` (min-heap) — a major improvement over the original sorted-vector approach.

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
│   │   │   ├── parsers/        # CSV parsers (Strategy pattern)
│   │   │   └── loaders/        # GraphLoader orchestrator
│   │   ├── api/
│   │   │   ├── controllers/    # Health, Station, Route endpoints
│   │   │   └── middleware/     # CORS filter
│   │   └── config/             # Environment-based configuration
│   └── tests/                  # Google Test unit tests
├── frontend/                   # Angular 19 (planned)
├── data/                       # RATP CSV data files
├── source/                     # Legacy C++ code (preserved)
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
- [Implementation Plan](IMPLEMENTATION_PLAN.md)

## Legacy Console App

The original console application is preserved in `source/`:

```bash
cd source
make
./main ../data/s.csv ../data/c.csv
```
* Adding a menu to choose between different data formats.
* Adding a menu to choose between different data structures.
* Adding an GUI.








  