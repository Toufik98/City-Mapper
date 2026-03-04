# City-Mapper REST API

## Base URL

- **Development**: `http://localhost:8080`
- **Docker**: `http://api.citymapper.localhost`

## Endpoints

### Health Check

```http
GET /health
```

**Response** `200 OK`:
```json
{
  "status": "ok",
  "service": "city-mapper-api",
  "version": "1.0.0",
  "graph": {
    "stations": 760,
    "connections": 2428
  },
  "cache": {
    "size": 42,
    "capacity": 10000,
    "hit_ratio": 0.85,
    "hits": 340,
    "misses": 60
  }
}
```

---

### List All Stations

```http
GET /api/stations
```

**Response** `200 OK`:
```json
{
  "count": 760,
  "stations": [
    {
      "id": 1642,
      "name": "Saint-Paul (Le Marais)",
      "line_id": "1",
      "line_name": "(CHATEAU DE VINCENNES <-> LA DEFENSE) - Aller",
      "address": "Rivoli (terre plein face au 10 rue de) - 75104",
      "latitude": 48.8553,
      "longitude": 2.3613
    }
  ]
}
```

---

### Get Station by ID

```http
GET /api/stations/{id}
```

**Response** `200 OK`:
```json
{
  "id": 1642,
  "name": "Saint-Paul (Le Marais)",
  "line_id": "1",
  "line_name": "(CHATEAU DE VINCENNES <-> LA DEFENSE) - Aller",
  "address": "Rivoli (terre plein face au 10 rue de) - 75104",
  "latitude": 48.8553,
  "longitude": 2.3613
}
```

**Response** `404 Not Found`:
```json
{
  "error": "Station not found",
  "id": 99999
}
```

---

### Search Stations

```http
GET /api/stations/search?q={query}&limit={limit}
```

| Parameter | Type   | Required | Default | Description                     |
|-----------|--------|----------|---------|---------------------------------|
| `q`       | string | yes      | —       | Search query (case-insensitive) |
| `limit`   | int    | no       | 10      | Max results (1–100)             |

**Response** `200 OK`:
```json
{
  "query": "chatelet",
  "count": 3,
  "stations": [
    {
      "id": 1768,
      "name": "Châtelet",
      "line_id": "1",
      ...
    }
  ]
}
```

---

### Compute Route

```http
POST /api/route
Content-Type: application/json
```

**Request Body**:
```json
{
  "from_id": 1642,
  "to_id": 1648,
  "algorithm": "a_star"
}
```

| Field       | Type   | Required | Default   | Description                        |
|-------------|--------|----------|-----------|------------------------------------|
| `from_id`   | uint64 | yes      | —         | Departure station ID               |
| `to_id`     | uint64 | yes      | —         | Arrival station ID                 |
| `algorithm` | string | no       | `a_star`  | `dijkstra` or `a_star`             |

**Response** `200 OK`:
```json
{
  "found": true,
  "cached": false,
  "algorithm": "A*",
  "total_duration_seconds": 1230,
  "total_duration_display": "20min 30s",
  "total_duration_compact": "21 min",
  "total_transfers": 2,
  "total_stations": 15,
  "computation_time_ms": 0.42,
  "segments": [
    {
      "from_station": {
        "id": 1642,
        "name": "Saint-Paul (Le Marais)",
        "line_id": "1",
        "latitude": 48.8553,
        "longitude": 2.3613
      },
      "to_station": {
        "id": 1769,
        "name": "Châtelet",
        "line_id": "1",
        "latitude": 48.8586,
        "longitude": 2.3470
      },
      "duration_seconds": 90,
      "duration_display": "1min 30s",
      "type": "RIDE",
      "line_id": "1",
      "line_name": "(CHATEAU DE VINCENNES <-> LA DEFENSE) - Aller"
    }
  ],
  "raw_path": [
    {"station_id": 1642, "cumulative_seconds": 0},
    {"station_id": 1769, "cumulative_seconds": 90}
  ]
}
```

**Response** `404 Not Found` (no route):
```json
{
  "found": false,
  "algorithm": "A*",
  "error": "No route found between the specified stations"
}
```

**Response** `400 Bad Request`:
```json
{
  "error": "Missing required fields: from_id, to_id"
}
```

---

## Error Codes

| HTTP Status | Meaning                                    |
|-------------|-------------------------------------------|
| `200`       | Success                                    |
| `400`       | Bad request (missing/invalid parameters)   |
| `404`       | Resource not found (station or route)      |
| `503`       | Service unavailable (graph not loaded)     |
