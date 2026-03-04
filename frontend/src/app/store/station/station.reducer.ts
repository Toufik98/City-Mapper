import { createReducer, on } from '@ngrx/store';
import { Station } from '../../core/models/station.model';
import * as StationActions from './station.actions';

export const stationFeatureKey = 'station';

export interface StationState {
  stations: Station[];
  searchResults: Station[];
  loading: boolean;
  searching: boolean;
  error: string | null;
}

export const initialStationState: StationState = {
  stations: [],
  searchResults: [],
  loading: false,
  searching: false,
  error: null,
};

export const stationReducer = createReducer(
  initialStationState,

  // Load stations
  on(StationActions.loadStations, (state) => ({
    ...state,
    loading: true,
    error: null,
  })),
  on(StationActions.loadStationsSuccess, (state, { stations }) => ({
    ...state,
    stations,
    loading: false,
  })),
  on(StationActions.loadStationsFailure, (state, { error }) => ({
    ...state,
    loading: false,
    error,
  })),

  // Search stations
  on(StationActions.searchStations, (state) => ({
    ...state,
    searching: true,
  })),
  on(StationActions.searchStationsSuccess, (state, { results }) => ({
    ...state,
    searchResults: results,
    searching: false,
  })),
  on(StationActions.searchStationsFailure, (state, { error }) => ({
    ...state,
    searching: false,
    error,
  })),

  // Clear search
  on(StationActions.clearSearch, (state) => ({
    ...state,
    searchResults: [],
    searching: false,
  }))
);
