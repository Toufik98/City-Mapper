import { createFeatureSelector, createSelector } from '@ngrx/store';
import { RouteState, routeFeatureKey } from './route.reducer';

export const selectRouteState = createFeatureSelector<RouteState>(routeFeatureKey);

export const selectDeparture = createSelector(
  selectRouteState,
  (state) => state.departure
);

export const selectArrival = createSelector(
  selectRouteState,
  (state) => state.arrival
);

export const selectAlgorithm = createSelector(
  selectRouteState,
  (state) => state.algorithm
);

export const selectCurrentRoute = createSelector(
  selectRouteState,
  (state) => state.currentRoute
);

export const selectRouteSegments = createSelector(
  selectCurrentRoute,
  (route) => route?.segments ?? []
);

export const selectTotalDuration = createSelector(
  selectCurrentRoute,
  (route) => route?.totalDurationSeconds ?? 0
);

export const selectTotalTransfers = createSelector(
  selectCurrentRoute,
  (route) => route?.totalTransfers ?? 0
);

export const selectRouteLoading = createSelector(
  selectRouteState,
  (state) => state.loading
);

export const selectRouteError = createSelector(
  selectRouteState,
  (state) => state.error
);

export const selectHasValidSelection = createSelector(
  selectDeparture,
  selectArrival,
  (departure, arrival) => departure !== null && arrival !== null
);
