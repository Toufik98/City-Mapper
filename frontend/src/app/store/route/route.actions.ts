import { createAction, props } from '@ngrx/store';
import { Station } from '../../core/models/station.model';
import { Algorithm, Route } from '../../core/models/route.model';

// Set departure / arrival
export const setDeparture = createAction(
  '[Route] Set Departure',
  props<{ station: Station }>()
);
export const setArrival = createAction(
  '[Route] Set Arrival',
  props<{ station: Station }>()
);
export const clearDeparture = createAction('[Route] Clear Departure');
export const clearArrival = createAction('[Route] Clear Arrival');

// Swap stations
export const swapStations = createAction('[Route] Swap Stations');

// Set algorithm
export const setAlgorithm = createAction(
  '[Route] Set Algorithm',
  props<{ algorithm: Algorithm }>()
);

// Compute route
export const computeRoute = createAction('[Route] Compute Route');
export const computeRouteSuccess = createAction(
  '[Route] Compute Route Success',
  props<{ route: Route }>()
);
export const computeRouteFailure = createAction(
  '[Route] Compute Route Failure',
  props<{ error: string }>()
);

// Clear route
export const clearRoute = createAction('[Route] Clear Route');
