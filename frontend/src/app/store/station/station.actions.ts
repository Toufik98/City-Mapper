import { createAction, props } from '@ngrx/store';
import { Station } from '../../core/models/station.model';

// Load all stations
export const loadStations = createAction('[Station] Load Stations');
export const loadStationsSuccess = createAction(
  '[Station] Load Stations Success',
  props<{ stations: Station[] }>()
);
export const loadStationsFailure = createAction(
  '[Station] Load Stations Failure',
  props<{ error: string }>()
);

// Search stations
export const searchStations = createAction(
  '[Station] Search Stations',
  props<{ query: string }>()
);
export const searchStationsSuccess = createAction(
  '[Station] Search Stations Success',
  props<{ results: Station[] }>()
);
export const searchStationsFailure = createAction(
  '[Station] Search Stations Failure',
  props<{ error: string }>()
);

// Clear search
export const clearSearch = createAction('[Station] Clear Search');
