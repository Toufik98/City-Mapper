import { Injectable, inject } from '@angular/core';
import { Observable, map } from 'rxjs';
import { ApiService } from './api.service';
import { Station } from '../models/station.model';

interface StationApiResponse {
  id: number;
  name: string;
  line_id: string;
  line_name: string;
  address: string;
  latitude: number;
  longitude: number;
}

/**
 * Service for station-related API calls.
 */
@Injectable({ providedIn: 'root' })
export class StationService {
  private readonly api = inject(ApiService);

  /**
   * Fetch all stations from the backend.
   */
  getAll(): Observable<Station[]> {
    return this.api.get<{ stations: StationApiResponse[] }>('/api/stations').pipe(
      map((response) => response.stations.map(this.mapStation))
    );
  }

  /**
   * Search stations by name with a query string.
   * @param query - Minimum 2 characters.
   * @param limit - Max results to return (default 10).
   */
  search(query: string, limit = 10): Observable<Station[]> {
    return this.api
      .get<{ stations: StationApiResponse[] }>('/api/stations/search', { q: query, limit })
      .pipe(map((response) => response.stations.map(this.mapStation)));
  }

  /**
   * Fetch a single station by ID.
   */
  getById(id: number): Observable<Station> {
    return this.api
      .get<{ station: StationApiResponse }>(`/api/stations/${id}`)
      .pipe(map((response) => this.mapStation(response.station)));
  }

  private mapStation(s: StationApiResponse): Station {
    return {
      id: s.id,
      name: s.name,
      lineId: s.line_id,
      lineName: s.line_name,
      address: s.address,
      latitude: s.latitude || null,
      longitude: s.longitude || null,
    };
  }
}
