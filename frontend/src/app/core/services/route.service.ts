import { Injectable, inject } from '@angular/core';
import { Observable, map } from 'rxjs';
import { ApiService } from './api.service';
import { Algorithm, Route } from '../models/route.model';
import { ConnectionType, RouteSegment } from '../models/segment.model';

interface RouteApiResponse {
  route: {
    segments: ApiSegment[];
    total_duration_seconds: number;
    total_transfers: number;
    algorithm: string;
    computation_time_ms: number;
  };
}

interface ApiSegment {
  from_station: { id: number; name: string };
  to_station: { id: number; name: string };
  duration_seconds: number;
  type: string;
  line_id: string | null;
  line_name: string | null;
}

/**
 * Service for route computation API calls.
 */
@Injectable({ providedIn: 'root' })
export class RouteService {
  private readonly api = inject(ApiService);

  /**
   * Compute the optimal route between two stations.
   */
  computeRoute(fromId: number, toId: number, algorithm: Algorithm): Observable<Route> {
    return this.api
      .post<RouteApiResponse>('/api/route', {
        from_id: fromId,
        to_id: toId,
        algorithm: algorithm,
      })
      .pipe(
        map((response) => ({
          segments: response.route.segments.map(this.mapSegment),
          totalDurationSeconds: response.route.total_duration_seconds,
          totalTransfers: response.route.total_transfers,
          algorithm: response.route.algorithm,
          computationTimeMs: response.route.computation_time_ms,
        }))
      );
  }

  private mapSegment(s: ApiSegment): RouteSegment {
    return {
      fromStation: { id: s.from_station.id, name: s.from_station.name },
      toStation: { id: s.to_station.id, name: s.to_station.name },
      durationSeconds: s.duration_seconds,
      type: (s.type as ConnectionType) ?? ConnectionType.RIDE,
      lineId: s.line_id,
      lineName: s.line_name,
    };
  }
}
