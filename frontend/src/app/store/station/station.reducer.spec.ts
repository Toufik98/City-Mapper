import { stationReducer, initialStationState, StationState } from './station.reducer';
import * as StationActions from './station.actions';
import { Station } from '../../core/models/station.model';

describe('Station Reducer', () => {
  it('should return the initial state', () => {
    const result = stationReducer(undefined, { type: 'unknown' });
    expect(result).toEqual(initialStationState);
  });

  it('should set loading on loadStations', () => {
    const result = stationReducer(
      initialStationState,
      StationActions.loadStations()
    );
    expect(result.loading).toBe(true);
    expect(result.error).toBeNull();
  });

  it('should populate stations on loadStationsSuccess', () => {
    const stations: Station[] = [
      { id: 1, name: 'Châtelet', lineId: '1', lineName: 'Ligne 1', address: '', latitude: 0, longitude: 0 },
      { id: 2, name: 'Nation', lineId: '1', lineName: 'Ligne 1', address: '', latitude: 0, longitude: 0 },
    ];
    const result = stationReducer(
      { ...initialStationState, loading: true },
      StationActions.loadStationsSuccess({ stations })
    );
    expect(result.stations).toEqual(stations);
    expect(result.loading).toBe(false);
    expect(result.error).toBeNull();
  });

  it('should set error on loadStationsFailure', () => {
    const result = stationReducer(
      { ...initialStationState, loading: true },
      StationActions.loadStationsFailure({ error: 'Network error' })
    );
    expect(result.loading).toBe(false);
    expect(result.error).toBe('Network error');
  });

  it('should set searching on searchStations', () => {
    const result = stationReducer(
      initialStationState,
      StationActions.searchStations({ query: 'Châ' })
    );
    expect(result.searching).toBe(true);
  });

  it('should populate search results on searchStationsSuccess', () => {
    const results: Station[] = [
      { id: 1, name: 'Châtelet', lineId: '1', lineName: 'Ligne 1', address: '', latitude: 0, longitude: 0 },
    ];
    const result = stationReducer(
      { ...initialStationState, searching: true },
      StationActions.searchStationsSuccess({ results })
    );
    expect(result.searchResults).toEqual(results);
    expect(result.searching).toBe(false);
  });

  it('should clear search results on clearSearch', () => {
    const state: StationState = {
      ...initialStationState,
      searchResults: [
        { id: 1, name: 'Châtelet', lineId: '1', lineName: 'Ligne 1', address: '', latitude: 0, longitude: 0 },
      ],
      searching: false,
    };
    const result = stationReducer(state, StationActions.clearSearch());
    expect(result.searchResults).toEqual([]);
  });
});
