# City-Mapper — Production Implementation Plan

> **Goal**: Transform the existing C++ Dijkstra console app into a production-grade, Google Maps-level Paris Metro route planner with an Angular 19 frontend and a C++ REST/GraphQL backend.

---

## Architecture Decisions Summary

| Decision | Choice |
|----------|--------|
| **C++ HTTP Framework** | Drogon |
| **Build System** | CMake |
| **C++ Standard** | C++20 |
| **JSON Library** | RapidJSON |
| **Algorithms** | Dijkstra + A* + Contraction Hierarchies + Pareto-optimal routing |
| **Data Source** | Current CSVs + RATP GTFS Open Data |
| **Caching** | In-memory LRU cache |
| **API Protocol** | GraphQL |
| **Map Library** | Leaflet + OpenStreetMap tiles |
| **Angular Version** | Angular 19 |
| **State Management** | NgRx (Store + Effects) |
| **UI Theme** | Material 3 — Metro-themed (Paris-inspired palette) |
| **Route Visualization** | Animated path tracing |
| **Station Selection** | Voice + Text autocomplete + Map click |
| **Frontend Testing** | Karma + Playwright |
| **Backend Testing** | Google Test + Google Mock |
| **Containerization** | Docker Compose + Traefik |
| **CI/CD** | GitHub Actions |
| **i18n** | Multi-language (ngx-translate) |
| **Authentication** | None — fully public |
| **Monitoring** | None for v1 |
| **API Docs** | GraphQL Playground / Apollo Sandbox |
| **Line Colors** | RATP official + accessibility toggle |
| **Mobile** | Responsive web only |

---

## Repository Structure (Target)

```
City-Mapper/
├── README.md
├── IMPLEMENTATION_PLAN.md
├── docker-compose.yml                # Orchestrates all services
├── docker-compose.dev.yml            # Development overrides
├── traefik/
│   ├── traefik.yml                   # Traefik static config
│   └── dynamic/
│       └── routes.yml                # Routing rules (FE, BE, Playground)
│
├── backend/                          # C++ Drogon GraphQL API
│   ├── CMakeLists.txt                # Root CMake config
│   ├── Dockerfile                    # Multi-stage build
│   ├── conanfile.txt                 # Conan dependencies (Drogon, RapidJSON, GTest)
│   ├── src/
│   │   ├── main.cpp                  # Drogon app entry point
│   │   ├── core/
│   │   │   ├── graph/
│   │   │   │   ├── Graph.hpp         # Adjacency list graph representation
│   │   │   │   ├── Graph.cpp
│   │   │   │   ├── Station.hpp       # Station model (id, name, lat, lng, line, address)
│   │   │   │   └── Connection.hpp    # Edge model (from, to, weight, type)
│   │   │   ├── algorithms/
│   │   │   │   ├── IPathfinder.hpp   # Pathfinder interface (Strategy pattern)
│   │   │   │   ├── Dijkstra.hpp      # Optimized Dijkstra (std::priority_queue)
│   │   │   │   ├── Dijkstra.cpp
│   │   │   │   ├── AStar.hpp         # A* with Haversine heuristic
│   │   │   │   ├── AStar.cpp
│   │   │   │   ├── ContractionHierarchies.hpp
│   │   │   │   ├── ContractionHierarchies.cpp
│   │   │   │   ├── ParetoRouter.hpp  # Multi-criteria optimization
│   │   │   │   └── ParetoRouter.cpp
│   │   │   ├── cache/
│   │   │   │   ├── LRUCache.hpp      # Template LRU cache
│   │   │   │   └── RouteCache.hpp    # Route-specific cache wrapper
│   │   │   └── utils/
│   │   │       ├── Haversine.hpp     # Geographic distance calculation
│   │   │       ├── TimeFormatter.hpp # Seconds → human-readable
│   │   │       └── StringUtils.hpp   # Case-insensitive comparison, trim
│   │   ├── data/
│   │   │   ├── parsers/
│   │   │   │   ├── IStationParser.hpp    # Parser interface
│   │   │   │   ├── IConnectionParser.hpp
│   │   │   │   ├── CSVStationParser.hpp  # Current CSV format
│   │   │   │   ├── CSVStationParser.cpp
│   │   │   │   ├── CSVConnectionParser.hpp
│   │   │   │   ├── CSVConnectionParser.cpp
│   │   │   │   ├── GTFSParser.hpp        # GTFS format support
│   │   │   │   └── GTFSParser.cpp
│   │   │   └── loaders/
│   │   │       ├── GraphLoader.hpp       # Orchestrates parsing → Graph
│   │   │       └── GraphLoader.cpp
│   │   ├── api/
│   │   │   ├── graphql/
│   │   │   │   ├── Schema.hpp            # GraphQL schema definition
│   │   │   │   ├── Schema.cpp
│   │   │   │   ├── resolvers/
│   │   │   │   │   ├── StationResolver.hpp
│   │   │   │   │   ├── StationResolver.cpp
│   │   │   │   │   ├── RouteResolver.hpp
│   │   │   │   │   └── RouteResolver.cpp
│   │   │   │   └── types/
│   │   │   │       ├── StationType.hpp
│   │   │   │       ├── RouteType.hpp
│   │   │   │       └── SegmentType.hpp
│   │   │   ├── controllers/
│   │   │   │   ├── GraphQLController.hpp # POST /graphql endpoint
│   │   │   │   ├── GraphQLController.cpp
│   │   │   │   ├── HealthController.hpp  # GET /health
│   │   │   │   └── HealthController.cpp
│   │   │   └── middleware/
│   │   │       ├── CORSMiddleware.hpp
│   │   │       └── CORSMiddleware.cpp
│   │   └── config/
│   │       ├── AppConfig.hpp             # Environment-based config
│   │       └── AppConfig.cpp
│   ├── tests/
│   │   ├── CMakeLists.txt
│   │   ├── unit/
│   │   │   ├── GraphTest.cpp
│   │   │   ├── DijkstraTest.cpp
│   │   │   ├── AStarTest.cpp
│   │   │   ├── ContractionHierarchiesTest.cpp
│   │   │   ├── ParetoRouterTest.cpp
│   │   │   ├── LRUCacheTest.cpp
│   │   │   ├── CSVParserTest.cpp
│   │   │   └── HaversineTest.cpp
│   │   ├── integration/
│   │   │   ├── RouteComputationTest.cpp  # End-to-end with real RATP data
│   │   │   └── GraphQLApiTest.cpp        # HTTP endpoint tests
│   │   └── fixtures/
│   │       ├── small_stations.csv
│   │       └── small_connections.csv
│   └── data/
│       ├── csv/
│       │   ├── stations.csv              # Current RATP data
│       │   └── connections.csv
│       └── gtfs/                         # RATP GTFS data
│           ├── stops.txt
│           ├── stop_times.txt
│           ├── routes.txt
│           ├── trips.txt
│           └── transfers.txt
│
├── frontend/                         # Angular 19 application
│   ├── Dockerfile                    # Multi-stage: build → nginx
│   ├── angular.json
│   ├── package.json
│   ├── tsconfig.json
│   ├── src/
│   │   ├── main.ts
│   │   ├── index.html
│   │   ├── styles.scss               # Global styles + Material 3 theme
│   │   ├── assets/
│   │   │   ├── i18n/
│   │   │   │   ├── en.json
│   │   │   │   ├── fr.json
│   │   │   │   └── ar.json           # Arabic (optional)
│   │   │   ├── icons/
│   │   │   │   └── metro-lines/      # SVG icons per line
│   │   │   └── images/
│   │   ├── app/
│   │   │   ├── app.component.ts
│   │   │   ├── app.config.ts         # Standalone app config
│   │   │   ├── app.routes.ts
│   │   │   ├── core/
│   │   │   │   ├── services/
│   │   │   │   │   ├── graphql.service.ts        # Apollo Client wrapper
│   │   │   │   │   ├── station.service.ts        # Station queries
│   │   │   │   │   ├── route.service.ts          # Route computation queries
│   │   │   │   │   ├── speech.service.ts         # Web Speech API wrapper
│   │   │   │   │   └── accessibility.service.ts  # Colorblind mode, a11y
│   │   │   │   ├── models/
│   │   │   │   │   ├── station.model.ts
│   │   │   │   │   ├── route.model.ts
│   │   │   │   │   ├── segment.model.ts
│   │   │   │   │   └── metro-line.model.ts
│   │   │   │   ├── constants/
│   │   │   │   │   ├── metro-lines.ts            # Line colors (official RATP)
│   │   │   │   │   └── metro-lines-accessible.ts # Colorblind-safe palette
│   │   │   │   └── interceptors/
│   │   │   │       └── error.interceptor.ts
│   │   │   ├── store/                             # NgRx
│   │   │   │   ├── station/
│   │   │   │   │   ├── station.actions.ts
│   │   │   │   │   ├── station.reducer.ts
│   │   │   │   │   ├── station.effects.ts
│   │   │   │   │   └── station.selectors.ts
│   │   │   │   ├── route/
│   │   │   │   │   ├── route.actions.ts
│   │   │   │   │   ├── route.reducer.ts
│   │   │   │   │   ├── route.effects.ts
│   │   │   │   │   └── route.selectors.ts
│   │   │   │   └── ui/
│   │   │   │       ├── ui.actions.ts
│   │   │   │       ├── ui.reducer.ts
│   │   │   │       └── ui.selectors.ts
│   │   │   ├── features/
│   │   │   │   ├── map/
│   │   │   │   │   ├── map.component.ts          # Leaflet map container
│   │   │   │   │   ├── map.component.html
│   │   │   │   │   ├── map.component.scss
│   │   │   │   │   ├── layers/
│   │   │   │   │   │   ├── station-layer.ts      # Station markers
│   │   │   │   │   │   ├── line-layer.ts         # Metro line polylines
│   │   │   │   │   │   └── route-layer.ts        # Animated route overlay
│   │   │   │   │   └── controls/
│   │   │   │   │       ├── zoom-control.ts
│   │   │   │   │       └── legend-control.ts     # Line color legend
│   │   │   │   ├── search/
│   │   │   │   │   ├── search-panel.component.ts         # Main search UI
│   │   │   │   │   ├── search-panel.component.html
│   │   │   │   │   ├── search-panel.component.scss
│   │   │   │   │   ├── station-autocomplete.component.ts # Autocomplete input
│   │   │   │   │   ├── voice-input.component.ts          # Voice recognition
│   │   │   │   │   └── swap-button.component.ts          # Swap start/end
│   │   │   │   ├── route-details/
│   │   │   │   │   ├── route-details.component.ts
│   │   │   │   │   ├── route-details.component.html
│   │   │   │   │   ├── route-details.component.scss
│   │   │   │   │   ├── segment-card.component.ts         # Individual segment
│   │   │   │   │   ├── transfer-indicator.component.ts   # Line change visual
│   │   │   │   │   └── route-summary.component.ts        # Total time, transfers
│   │   │   │   └── settings/
│   │   │   │       ├── settings-panel.component.ts
│   │   │   │       ├── language-selector.component.ts
│   │   │   │       └── accessibility-toggle.component.ts
│   │   │   └── shared/
│   │   │       ├── components/
│   │   │       │   ├── metro-line-badge.component.ts     # Colored circle with line number
│   │   │       │   ├── station-chip.component.ts         # Station name + line badge
│   │   │       │   ├── loading-spinner.component.ts
│   │   │       │   └── error-toast.component.ts
│   │   │       ├── pipes/
│   │   │       │   ├── duration.pipe.ts                  # Seconds → "12 min"
│   │   │       │   └── station-name.pipe.ts              # Formatting
│   │   │       └── directives/
│   │   │           └── click-outside.directive.ts
│   │   └── environments/
│   │       ├── environment.ts
│   │       └── environment.prod.ts
│   └── e2e/
│       ├── playwright.config.ts
│       └── specs/
│           ├── search-route.spec.ts
│           ├── map-interaction.spec.ts
│           └── accessibility.spec.ts
│
├── data/                             # Legacy data (preserved)
│   ├── s.csv
│   ├── c.csv
│   ├── stations.csv
│   └── connections.csv
│
├── source/                           # Legacy C++ code (preserved for reference)
│   └── ... (existing files)
│
├── .github/
│   └── workflows/
│       ├── backend-ci.yml            # Build, test, lint C++ backend
│       ├── frontend-ci.yml           # Build, test, lint Angular frontend
│       └── deploy.yml                # Docker build + push to registry
│
└── docs/
    ├── ARCHITECTURE.md               # System architecture overview
    ├── API_SCHEMA.md                 # GraphQL schema documentation
    ├── DATA_MODEL.md                 # Graph data model
    ├── ALGORITHMS.md                 # Algorithm explanations + complexity
    └── DEPLOYMENT.md                 # How to deploy
```

---

## Phase 1: Backend Foundation (Weeks 1–3)

### 1.1 — Project Setup & Build System
- [ ] Create `backend/` directory with CMake project structure
- [ ] Configure Conan package manager for dependencies:
  - `drogon/1.9.x` — HTTP framework
  - `rapidjson/1.1.x` — JSON serialization
  - `gtest/1.14.x` — Testing framework
  - `spdlog/1.12.x` — Logging
- [ ] Set up CMake with C++20, proper target definitions, CTest integration
- [ ] Create multi-stage Dockerfile (build with gcc/clang → runtime with minimal image)
- [ ] Migrate from Makefile to CMake with backward compatibility

### 1.2 — Data Model Redesign
- [ ] Design new `Station` model with geographic coordinates:
  ```cpp
  struct Station {
      uint64_t id;
      std::string name;
      std::string line_id;
      std::string line_name;
      std::string address;
      double latitude;    // NEW: from GTFS
      double longitude;   // NEW: from GTFS
      // operator==, hash for use in unordered containers
  };
  ```
- [ ] Design `Connection` model with edge type semantics:
  ```cpp
  struct Connection {
      uint64_t from_id;
      uint64_t to_id;
      uint64_t weight_seconds;
      ConnectionType type;  // RIDE, TRANSFER, WALK
  };
  ```
- [ ] Design `Graph` class:
  ```cpp
  class Graph {
      std::unordered_map<uint64_t, Station> stations;
      std::unordered_map<uint64_t, std::vector<WeightedEdge>> adjacency;
      // O(1) station lookup, O(1) neighbor access
  };
  ```
- [ ] Write comprehensive unit tests for all models

### 1.3 — Data Parsers
- [ ] Implement `IStationParser` / `IConnectionParser` interfaces (Strategy pattern)
- [ ] Implement `CSVStationParser` — fix the off-by-one EOF bug from the original
- [ ] Implement `CSVConnectionParser` — fix the off-by-one EOF bug
- [ ] Implement `GTFSParser` — parse RATP GTFS open data files:
  - `stops.txt` → stations with lat/lng
  - `transfers.txt` → transfer connections
  - `stop_times.txt` + `trips.txt` → ride connections with real schedules
- [ ] Implement `GraphLoader` that orchestrates parsers → builds `Graph`
- [ ] Download and integrate RATP GTFS data from https://data.ratp.fr/
- [ ] Unit tests for all parsers with fixture files

### 1.4 — Algorithm Engine (Core)
- [ ] Define `IPathfinder` interface (Strategy pattern):
  ```cpp
  class IPathfinder {
  public:
      virtual ~IPathfinder() = default;
      virtual RouteResult compute(const Graph& graph, uint64_t start, uint64_t end) = 0;
      virtual std::string name() const = 0;
  };
  ```
- [ ] Implement **optimized Dijkstra**:
  - `std::priority_queue<pair<uint64_t, uint64_t>, vector, greater>` (min-heap)
  - `std::unordered_set<uint64_t>` for visited nodes → O(1) lookup
  - `std::unordered_map<uint64_t, uint64_t>` for parent tracking
  - **Target complexity: O((V + E) log V)**
- [ ] Implement **A*** with Haversine heuristic:
  - Admissible heuristic: geographic straight-line distance / max metro speed
  - Requires lat/lng data from GTFS
  - Falls back to Dijkstra if coordinates unavailable
- [ ] Implement **Contraction Hierarchies**:
  - Preprocessing phase: contract nodes by importance (degree, edge difference)
  - Query phase: bidirectional Dijkstra on contracted graph
  - **Near-instant queries** after O(V log V) preprocessing
- [ ] Implement **Pareto-optimal router**:
  - Multi-criteria: minimize (time, number_of_transfers, walking_distance)
  - Returns Pareto frontier of non-dominated routes
  - User picks preferred trade-off on the frontend
- [ ] Unit tests: correctness against known shortest paths in small test graph
- [ ] Integration tests: verify results on full RATP dataset
- [ ] Benchmark tests: measure latency percentiles (p50, p95, p99)

### 1.5 — LRU Cache
- [ ] Implement generic `LRUCache<Key, Value>` template:
  - `std::list` for LRU ordering + `std::unordered_map` for O(1) lookup
  - Configurable max size (default: 10,000 entries)
  - Thread-safe with `std::shared_mutex` (read-write lock)
- [ ] Implement `RouteCache` wrapping LRU for route-specific caching:
  - Key: `{start_id, end_id, algorithm}`
  - Value: `RouteResult` with TTL
- [ ] Unit tests for eviction, thread safety, hit/miss ratios

### 1.6 — GraphQL API Layer
- [ ] Integrate a C++ GraphQL library (e.g., `cppgraphqlgen` by Microsoft)
- [ ] Define GraphQL schema:
  ```graphql
  type Station {
    id: ID!
    name: String!
    lineId: String!
    lineName: String!
    address: String!
    latitude: Float
    longitude: Float
  }

  type Segment {
    fromStation: Station!
    toStation: Station!
    durationSeconds: Int!
    type: ConnectionType!
    lineId: String
    lineName: String
  }

  type Route {
    segments: [Segment!]!
    totalDurationSeconds: Int!
    totalTransfers: Int!
    algorithm: String!
  }

  enum ConnectionType {
    RIDE
    TRANSFER
    WALK
  }

  enum Algorithm {
    DIJKSTRA
    A_STAR
    CONTRACTION_HIERARCHIES
    PARETO
  }

  type Query {
    stations: [Station!]!
    station(id: ID!): Station
    searchStations(query: String!, limit: Int = 10): [Station!]!
    route(
      fromId: ID!
      toId: ID!
      algorithm: Algorithm = A_STAR
    ): Route!
    paretoRoutes(
      fromId: ID!
      toId: ID!
    ): [Route!]!
  }
  ```
- [ ] Implement resolvers:
  - `StationResolver`: station lookup, fuzzy search by name
  - `RouteResolver`: delegates to pathfinder engine, caches results
- [ ] Implement Drogon controller for `POST /graphql`
- [ ] CORS middleware for frontend access
- [ ] Health endpoint: `GET /health` returning service status + graph stats
- [ ] Integration tests for all GraphQL queries

---

## Phase 2: Frontend Foundation (Weeks 3–5)

### 2.1 — Angular Project Setup
- [ ] Generate Angular 19 project with standalone components:
  ```bash
  ng new city-mapper-frontend --style=scss --routing --standalone
  ```
- [ ] Install and configure:
  - `@angular/material` (Material 3 / MDC)
  - `@angular/cdk`
  - `@ngrx/store`, `@ngrx/effects`, `@ngrx/store-devtools`
  - `apollo-angular` + `@apollo/client` (GraphQL client)
  - `leaflet` + `@types/leaflet`
  - `@ngx-translate/core` + `@ngx-translate/http-loader`
- [ ] Configure Material 3 custom theme with Paris Metro palette:
  ```scss
  // Primary: RATP Blue (#003B8E)
  // Secondary: Metro Green (#00A86B)  
  // Tertiary: Warm Gold (#FFB800)
  // Error: Alert Red (#E63946)
  // Surface: Soft off-white (#FAFAFA)
  // On-surface: Charcoal (#1D1D1D)
  ```
- [ ] Set up `environment.ts` files for dev/prod API URLs
- [ ] Configure Apollo Client with GraphQL endpoint
- [ ] Set up ngx-translate with French + English JSON files
- [ ] Create Dockerfile: `ng build --configuration production` → nginx

### 2.2 — NgRx State Management
- [ ] **Station Store**:
  - State: `{ stations: Station[], loading: boolean, error: string | null, searchResults: Station[] }`
  - Actions: `loadStations`, `loadStationsSuccess`, `searchStations`, `searchStationsSuccess`
  - Effects: GraphQL queries on action dispatch
  - Selectors: `selectAllStations`, `selectStationById`, `selectSearchResults`
- [ ] **Route Store**:
  - State: `{ currentRoute: Route | null, paretoRoutes: Route[], departure: Station | null, arrival: Station | null, algorithm: Algorithm, loading: boolean }`
  - Actions: `setDeparture`, `setArrival`, `computeRoute`, `computeRouteSuccess`, `swapStations`
  - Effects: Route computation via GraphQL
  - Selectors: `selectCurrentRoute`, `selectRouteSegments`, `selectTotalDuration`
- [ ] **UI Store**:
  - State: `{ language: 'fr' | 'en', accessibilityMode: boolean, sidebarOpen: boolean, mapZoom: number }`
  - Actions: `setLanguage`, `toggleAccessibility`, `toggleSidebar`
  - Selectors: `selectLanguage`, `selectIsAccessible`
- [ ] Configure NgRx DevTools for development

### 2.3 — Core Services
- [ ] `GraphQLService` — Apollo Client wrapper with typed queries/mutations
- [ ] `StationService`:
  - `getAll(): Observable<Station[]>`
  - `search(query: string): Observable<Station[]>` — debounced, min 2 chars
  - `getById(id: string): Observable<Station>`
- [ ] `RouteService`:
  - `computeRoute(from: string, to: string, algo: Algorithm): Observable<Route>`
  - `computeParetoRoutes(from: string, to: string): Observable<Route[]>`
- [ ] `SpeechService` — Web Speech API wrapper:
  - `startListening(): Observable<string>` — returns recognized text
  - `isSupported(): boolean` — feature detection
  - Browser compatibility handling (Chrome, Edge, Safari)
- [ ] `AccessibilityService`:
  - `toggleColorblindMode(): void`
  - `getLineColor(lineId: string): string` — returns appropriate color based on mode

### 2.4 — Constants & Models
- [ ] RATP official line colors:
  ```typescript
  export const METRO_LINE_COLORS: Record<string, string> = {
    '1': '#FFCD00',   // Yellow
    '2': '#003CA6',   // Blue
    '3': '#837902',   // Olive
    '3bis': '#6EC4E8', // Light Blue
    '4': '#CF009E',   // Magenta
    '5': '#FF7E2E',   // Orange
    '6': '#6ECA97',   // Light Green
    '7': '#FA9ABA',   // Pink
    '7bis': '#6ECA97', // Light Green
    '8': '#E19BDF',   // Lilac
    '9': '#B6BD00',   // Yellow-Green
    '10': '#C9910D',  // Brown-Gold
    '11': '#704B1C',  // Brown
    '12': '#007852',  // Dark Green
    '13': '#6EC4E8',  // Light Blue
    '14': '#62259D',  // Purple
  };
  ```
- [ ] Colorblind-safe alternative palette (patterns + contrasting colors)
- [ ] TypeScript interfaces matching GraphQL schema

---

## Phase 3: Feature Implementation (Weeks 5–8)

### 3.1 — Interactive Map (Leaflet)
- [ ] `MapComponent` — full-screen Leaflet map centered on Paris:
  - Center: `[48.8566, 2.3522]` (Paris)
  - Default zoom: 12 (city-wide view of metro network)
  - Tile layer: OpenStreetMap Carto (light theme) or CartoDB Positron (minimal clean look)
- [ ] `StationLayer`:
  - Render all ~760 stations as circle markers
  - Color by metro line (RATP official colors)
  - Size scales with zoom level (small at city-wide, larger when zoomed in)
  - Click handler: dispatches `setDeparture` / `setArrival` action
  - Tooltip on hover: station name + line
  - Cluster stations at low zoom levels (`markercluster` plugin)
- [ ] `LineLayer`:
  - Render metro line polylines connecting stations
  - Colored by line (matching station colors)
  - Dashed style for transfers/walking connections
  - Toggle individual lines on/off (legend control)
- [ ] `RouteLayer`:
  - Animated polyline showing computed route
  - Moving marker (metro icon) tracing the path
  - Segment coloring matches the metro line used
  - Transfer points highlighted with a walking icon
  - Animation speed: configurable, default ~2 seconds per segment
  - Glow/pulse effect on the active segment
- [ ] `LegendControl` — custom Leaflet control showing line colors  
- [ ] Responsive: full-screen on mobile, side-panel on desktop

### 3.2 — Station Search Panel
- [ ] `SearchPanelComponent`:
  - Floating panel (Material 3 `mat-card` with elevation) overlaying the map
  - Two `StationAutocompleteComponent` inputs: departure + arrival
  - "Swap" button (animated rotation icon) between inputs
  - "Find Route" primary action button (Material 3 FAB or raised button)
  - Algorithm selector (hidden in "Advanced" expandable section):
    - Radio buttons: Dijkstra, A*, Contraction Hierarchies, Pareto
  - Collapsible on mobile (FAB to open)
- [ ] `StationAutocompleteComponent`:
  - `mat-autocomplete` with debounced search (300ms)
  - Fuzzy matching on station name
  - Results show station name + line badge (colored circle)
  - Keyboard navigation (arrow keys, enter to select)
  - Clear button (X icon)
  - Integrates with NgRx station store
- [ ] `VoiceInputComponent`:
  - Microphone icon button next to each station input
  - Uses Web Speech API (`SpeechRecognition`)
  - Animated recording indicator (pulsing red dot)
  - Recognized text flows into autocomplete
  - French and English language support
  - Graceful fallback: hidden if browser doesn't support Speech API

### 3.3 — Route Details Panel
- [ ] `RouteDetailsComponent`:
  - Slides in from the right (or bottom on mobile) when route computed
  - Material 3 card with route summary at top:
    - Total duration (bold, large)
    - Number of transfers
    - Number of stations
    - Algorithm used (chip badge)
  - Scrollable list of `SegmentCardComponent`
- [ ] `SegmentCardComponent`:
  - Visual: colored left border matching metro line
  - Content: "Line X: Station A → Station B (5 min, 3 stops)"
  - Metro line badge (colored circle with number)
  - Tap to zoom map to that segment
- [ ] `TransferIndicatorComponent`:
  - Walking icon between segments
  - "Walk to Line X platform (2 min)"
  - Dotted connector between segment cards
- [ ] `RouteSummaryComponent`:
  - For Pareto results: show multiple route cards
  - Each card shows trade-offs: "Fastest (25 min, 3 transfers)" vs "Fewest transfers (30 min, 1 transfer)"
  - User taps to select and animate on map

### 3.4 — Settings & Accessibility
- [ ] `SettingsPanelComponent`:
  - Accessible via gear icon in top bar
  - Material 3 bottom sheet on mobile, side panel on desktop
- [ ] `LanguageSelectorComponent`:
  - ngx-translate language switch: FR / EN / (others)
  - Flag icons or language name buttons
  - Persisted in localStorage
- [ ] `AccessibilityToggleComponent`:
  - Toggle switch for colorblind mode
  - Swaps line color palette to colorblind-safe version
  - Adds pattern fills (stripes, dots, dashes) to line polylines
  - Persisted in localStorage

---

## Phase 4: Polish & Production (Weeks 8–10)

### 4.1 — Performance Optimization
- [ ] Backend:
  - Contraction Hierarchies preprocessing at startup (~2-5 seconds for RATP graph)
  - LRU cache warming with top-50 most popular station pairs
  - Response compression (gzip/brotli via Drogon)
  - Connection pooling (if adding DB later)
- [ ] Frontend:
  - Lazy load route-details and settings modules
  - `OnPush` change detection on all components
  - `trackBy` functions on all `*ngFor` / `@for` loops
  - Virtual scrolling for station list if > 100 results
  - Preload station data on app init (NgRx effect)
  - Leaflet: canvas renderer instead of SVG for large marker counts
  - Bundle size audit: target < 300KB initial load (gzipped)

### 4.2 — Responsive Design
- [ ] Breakpoints:
  - Mobile (< 768px): Full-screen map, floating search FAB, bottom sheet for details
  - Tablet (768px–1024px): Map with collapsible side panel
  - Desktop (> 1024px): Map with always-visible side panel (400px wide)
- [ ] Touch gestures: pinch-to-zoom on map, swipe to dismiss panels
- [ ] Safe area handling for notched devices

### 4.3 — Error Handling
- [ ] Frontend: global error interceptor → Material 3 snackbar notifications
- [ ] "No route found" state with helpful message
- [ ] "Backend unavailable" state with retry button
- [ ] Offline fallback: display cached station list, show "offline" banner
- [ ] 404 page with map background

### 4.4 — Testing
- [ ] Backend unit tests: ≥ 90% coverage on algorithm engine
- [ ] Backend integration tests: full API flow with test fixtures
- [ ] Frontend Karma tests: all services, store reducers, selectors, effects
- [ ] Playwright E2E:
  - Search and compute a route end-to-end
  - Verify map animation plays
  - Test voice input (mocked)
  - Test accessibility mode toggle
  - Test language switching
  - Test mobile responsive layout

### 4.5 — Docker & Deployment
- [ ] `docker-compose.yml`:
  ```yaml
  services:
    traefik:
      image: traefik:v3
      ports: ["80:80", "443:443"]
      volumes:
        - ./traefik/traefik.yml:/etc/traefik/traefik.yml
        - ./traefik/dynamic:/etc/traefik/dynamic
        - /var/run/docker.sock:/var/run/docker.sock
    
    backend:
      build: ./backend
      labels:
        - "traefik.http.routers.api.rule=Host(`api.citymapper.local`)"
      environment:
        - DATA_PATH=/data
        - CACHE_SIZE=10000
    
    frontend:
      build: ./frontend
      labels:
        - "traefik.http.routers.web.rule=Host(`citymapper.local`)"
  ```
- [ ] Traefik config: automatic HTTPS (Let's Encrypt), rate limiting, headers
- [ ] `docker-compose.dev.yml` overrides: hot-reload volumes, debug ports

### 4.6 — GitHub Actions CI/CD
- [ ] `backend-ci.yml`:
  - Trigger: push/PR to `main` or `backend/**`
  - Steps: Conan install → CMake build → CTest → gcov coverage report
  - Matrix: gcc-13, clang-17
- [ ] `frontend-ci.yml`:
  - Trigger: push/PR to `main` or `frontend/**`
  - Steps: npm ci → ng lint → ng test --code-coverage → ng build --prod → Playwright E2E
- [ ] `deploy.yml`:
  - Trigger: tag `v*`
  - Steps: Docker build → push to GitHub Container Registry → deploy notification

---

## Phase 5: Advanced Features (Weeks 10–12, Optional)

- [ ] **Real-time disruptions**: Integrate RATP real-time API for service status
- [ ] **Recent searches**: LocalStorage-based history with quick re-search
- [ ] **Share route**: Generate shareable URL with from/to parameters
- [ ] **Embeddable widget**: `<iframe>` version for other websites
- [ ] **Dark mode**: Material 3 dark theme toggle
- [ ] **Print route**: Printable step-by-step directions
- [ ] **Keyboard shortcuts**: `/` to focus search, `Esc` to clear, `Enter` to compute

---

## Timeline Summary

| Phase | Duration | Deliverable |
|-------|----------|-------------|
| **Phase 1** — Backend Foundation | Weeks 1–3 | C++ GraphQL API with 4 algorithms, tested |
| **Phase 2** — Frontend Foundation | Weeks 3–5 | Angular 19 app with NgRx, Apollo, Material 3 shell |
| **Phase 3** — Feature Implementation | Weeks 5–8 | Interactive map, search, route display, voice, a11y |
| **Phase 4** — Polish & Production | Weeks 8–10 | Performance, responsive, testing, Docker, CI/CD |
| **Phase 5** — Advanced Features | Weeks 10–12 | Real-time data, sharing, dark mode, extras |

---

## Key Technical Risks & Mitigations

| Risk | Mitigation |
|------|------------|
| C++ GraphQL libraries are immature | Fall back to REST endpoints (`POST /api/route`) and keep GraphQL as a wrapper layer |
| Drogon learning curve | Strong documentation + similar to Express.js patterns. Alternative: Crow for simplicity |
| RATP GTFS data quality | Validate + clean data with unit tests. Keep CSV fallback for core functionality |
| Contraction Hierarchies complexity | Implement last. Dijkstra + A* already provide great performance for ~760 nodes |
| Web Speech API browser support | Feature detection + graceful hidden fallback. Works in Chrome/Edge (>80% of users) |
| Leaflet animation performance | Canvas renderer + requestAnimationFrame. Limit marker count at low zoom with clustering |
