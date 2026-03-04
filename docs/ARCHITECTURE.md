# City-Mapper — Architecture Overview

## System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                      Client (Browser)                    │
│                                                          │
│    Angular 19 + Material 3 + Leaflet + NgRx + Apollo    │
└──────────────────────────┬───────────────────────────────┘
                           │ HTTP/JSON (REST)
                           │
┌──────────────────────────┼───────────────────────────────┐
│                     Traefik v3                           │
│                  (Reverse Proxy)                          │
│   citymapper.localhost → frontend                        │
│   api.citymapper.localhost → backend                     │
└──────────────────────────┼───────────────────────────────┘
                           │
         ┌─────────────────┴──────────────────┐
         │                                    │
         ▼                                    ▼
┌─────────────────┐              ┌─────────────────────┐
│  Backend (C++)  │              │  Frontend (Angular)  │
│  Drogon HTTP    │              │  nginx (production)  │
│  Port 8080      │              │  Port 80             │
└────────┬────────┘              └─────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│                   Application Layer                       │
│                                                          │
│  ┌──────────────┐  ┌──────────────┐  ┌────────────────┐ │
│  │    Health     │  │   Station    │  │     Route      │ │
│  │  Controller   │  │  Controller  │  │   Controller   │ │
│  └──────────────┘  └──────────────┘  └───────┬────────┘ │
│                                              │          │
│  ┌───────────────────────────────────────────┼────────┐ │
│  │              Route Cache (LRU)            │        │ │
│  │            10,000 entries, O(1)           │        │ │
│  └───────────────────────────────────────────┼────────┘ │
│                                              │          │
│  ┌───────────────────────────────────────────┼────────┐ │
│  │           Pathfinding Engine              │        │ │
│  │  ┌──────────┐  ┌──────────┐             │        │ │
│  │  │ Dijkstra │  │   A*     │  (Strategy)  │        │ │
│  │  └──────────┘  └──────────┘             │        │ │
│  └───────────────────────────────────────────┼────────┘ │
│                                              │          │
│  ┌───────────────────────────────────────────┼────────┐ │
│  │              Graph (Core)                 │        │ │
│  │   unordered_map<id, Station>             │        │ │
│  │   unordered_map<id, vector<Edge>>        │        │ │
│  │   ~760 nodes, ~2428 edges                │        │ │
│  └───────────────────────────────────────────┼────────┘ │
│                                              │          │
│  ┌───────────────────────────────────────────┼────────┐ │
│  │        Data Layer (Parsers)               │        │ │
│  │  ┌────────────┐  ┌─────────────────────┐ │        │ │
│  │  │ CSV Parser │  │ GraphLoader         │ │        │ │
│  │  └────────────┘  └─────────────────────┘ │        │ │
│  └──────────────────────────────────────────────────────┘ │
│                                                          │
│  ┌──────────────────────────────────────────────────────┐ │
│  │              Data Files (CSV)                        │ │
│  │  s.csv (760 stations)  c.csv (2428 connections)     │ │
│  └──────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

## Design Patterns Used

| Pattern   | Where                               | Purpose                                    |
|-----------|-------------------------------------|--------------------------------------------|
| Strategy  | `IPathfinder` → Dijkstra, A*       | Swap algorithms at runtime                 |
| Strategy  | `IStationParser`, `IConnectionParser` | Swap data formats (CSV, GTFS)          |
| Singleton | Global `Graph*`, `RouteCache*`      | Single graph instance shared by controllers |
| Template  | `LRUCache<K, V>`                   | Generic, reusable caching                  |
| MVC       | Controllers → Graph → JSON response | Separation of concerns                     |

## Complexity Summary

| Operation              | Complexity         | Implementation          |
|------------------------|--------------------|-------------------------|
| Station lookup by ID   | O(1) amortized     | `unordered_map`         |
| Neighbor access        | O(1) amortized     | `unordered_map<vector>` |
| Dijkstra shortest path | O((V+E) log V)     | `priority_queue`        |
| A* shortest path       | O((V+E) log V)     | `priority_queue` + Haversine |
| Station search by name | O(n)               | Linear scan + sort      |
| Cache get/put          | O(1) amortized     | `list` + `unordered_map`|
| CSV parsing            | O(n)               | Single-pass stream      |
