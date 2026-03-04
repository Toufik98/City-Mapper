import { createReducer, on } from '@ngrx/store';
import { Station } from '../../core/models/station.model';
import { Algorithm, Route } from '../../core/models/route.model';
import * as RouteActions from './route.actions';

export const routeFeatureKey = 'route';

export interface RouteState {
  departure: Station | null;
  arrival: Station | null;
  algorithm: Algorithm;
  currentRoute: Route | null;
  loading: boolean;
  error: string | null;
}

export const initialRouteState: RouteState = {
  departure: null,
  arrival: null,
  algorithm: Algorithm.A_STAR,
  currentRoute: null,
  loading: false,
  error: null,
};

export const routeReducer = createReducer(
  initialRouteState,

  // Departure / Arrival
  on(RouteActions.setDeparture, (state, { station }) => ({
    ...state,
    departure: station,
    currentRoute: null,
    error: null,
  })),
  on(RouteActions.setArrival, (state, { station }) => ({
    ...state,
    arrival: station,
    currentRoute: null,
    error: null,
  })),
  on(RouteActions.clearDeparture, (state) => ({
    ...state,
    departure: null,
    currentRoute: null,
  })),
  on(RouteActions.clearArrival, (state) => ({
    ...state,
    arrival: null,
    currentRoute: null,
  })),

  // Swap
  on(RouteActions.swapStations, (state) => ({
    ...state,
    departure: state.arrival,
    arrival: state.departure,
    currentRoute: null,
  })),

  // Algorithm
  on(RouteActions.setAlgorithm, (state, { algorithm }) => ({
    ...state,
    algorithm,
  })),

  // Compute route
  on(RouteActions.computeRoute, (state) => ({
    ...state,
    loading: true,
    error: null,
    currentRoute: null,
  })),
  on(RouteActions.computeRouteSuccess, (state, { route }) => ({
    ...state,
    currentRoute: route,
    loading: false,
  })),
  on(RouteActions.computeRouteFailure, (state, { error }) => ({
    ...state,
    loading: false,
    error,
  })),

  // Clear
  on(RouteActions.clearRoute, (state) => ({
    ...state,
    currentRoute: null,
    error: null,
  }))
);
