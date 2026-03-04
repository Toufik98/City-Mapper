import { routeReducer, initialRouteState, RouteState } from './route.reducer';
import * as RouteActions from './route.actions';
import { Algorithm } from '../../core/models/route.model';

const mockStation = (id: number, name: string) => ({
  id,
  name,
  lineId: '1',
  lineName: 'Ligne 1',
  address: '',
  latitude: 48.8566,
  longitude: 2.3522,
});

const mockRoute = {
  segments: [],
  totalDurationSeconds: 600,
  totalTransfers: 1,
  algorithm: 'astar',
  computationTimeMs: 5,
};

describe('Route Reducer', () => {
  it('should return the initial state', () => {
    const result = routeReducer(undefined, { type: 'unknown' });
    expect(result).toEqual(initialRouteState);
  });

  it('should set departure on setDeparture', () => {
    const station = mockStation(1, 'Châtelet');
    const result = routeReducer(
      initialRouteState,
      RouteActions.setDeparture({ station })
    );
    expect(result.departure).toEqual(station);
    expect(result.currentRoute).toBeNull();
  });

  it('should set arrival on setArrival', () => {
    const station = mockStation(2, 'Nation');
    const result = routeReducer(
      initialRouteState,
      RouteActions.setArrival({ station })
    );
    expect(result.arrival).toEqual(station);
  });

  it('should swap departure and arrival', () => {
    const dep = mockStation(1, 'Châtelet');
    const arr = mockStation(2, 'Nation');
    const state: RouteState = {
      ...initialRouteState,
      departure: dep,
      arrival: arr,
    };
    const result = routeReducer(state, RouteActions.swapStations());
    expect(result.departure).toEqual(arr);
    expect(result.arrival).toEqual(dep);
  });

  it('should clear departure', () => {
    const state: RouteState = {
      ...initialRouteState,
      departure: mockStation(1, 'Châtelet'),
    };
    const result = routeReducer(state, RouteActions.clearDeparture());
    expect(result.departure).toBeNull();
  });

  it('should clear arrival', () => {
    const state: RouteState = {
      ...initialRouteState,
      arrival: mockStation(2, 'Nation'),
    };
    const result = routeReducer(state, RouteActions.clearArrival());
    expect(result.arrival).toBeNull();
  });

  it('should set algorithm', () => {
    const result = routeReducer(
      initialRouteState,
      RouteActions.setAlgorithm({ algorithm: Algorithm.DIJKSTRA })
    );
    expect(result.algorithm).toBe(Algorithm.DIJKSTRA);
  });

  it('should set loading on computeRoute', () => {
    const result = routeReducer(
      initialRouteState,
      RouteActions.computeRoute()
    );
    expect(result.loading).toBe(true);
    expect(result.error).toBeNull();
    expect(result.currentRoute).toBeNull();
  });

  it('should set route on computeRouteSuccess', () => {
    const result = routeReducer(
      { ...initialRouteState, loading: true },
      RouteActions.computeRouteSuccess({ route: mockRoute })
    );
    expect(result.currentRoute).toEqual(mockRoute);
    expect(result.loading).toBe(false);
  });

  it('should set error on computeRouteFailure', () => {
    const result = routeReducer(
      { ...initialRouteState, loading: true },
      RouteActions.computeRouteFailure({ error: 'No route found' })
    );
    expect(result.error).toBe('No route found');
    expect(result.loading).toBe(false);
  });

  it('should clear route on clearRoute', () => {
    const state: RouteState = {
      ...initialRouteState,
      currentRoute: mockRoute,
    };
    const result = routeReducer(state, RouteActions.clearRoute());
    expect(result.currentRoute).toBeNull();
  });
});
