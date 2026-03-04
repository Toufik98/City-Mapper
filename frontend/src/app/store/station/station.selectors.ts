import { createFeatureSelector, createSelector } from '@ngrx/store';
import { StationState, stationFeatureKey } from './station.reducer';

export const selectStationState = createFeatureSelector<StationState>(stationFeatureKey);

export const selectAllStations = createSelector(
  selectStationState,
  (state) => state.stations
);

export const selectStationsLoading = createSelector(
  selectStationState,
  (state) => state.loading
);

export const selectSearchResults = createSelector(
  selectStationState,
  (state) => state.searchResults
);

export const selectSearching = createSelector(
  selectStationState,
  (state) => state.searching
);

export const selectStationError = createSelector(
  selectStationState,
  (state) => state.error
);

export const selectStationById = (id: number) =>
  createSelector(selectAllStations, (stations) =>
    stations.find((s) => s.id === id) ?? null
  );
