import { RouteSegment } from './segment.model';

/**
 * Available pathfinding algorithms.
 */
export enum Algorithm {
  DIJKSTRA = 'dijkstra',
  A_STAR = 'astar',
  CONTRACTION_HIERARCHIES = 'contraction_hierarchies',
  PARETO = 'pareto',
}

/**
 * Result of a route computation from the backend.
 */
export interface Route {
  readonly segments: RouteSegment[];
  readonly totalDurationSeconds: number;
  readonly totalTransfers: number;
  readonly algorithm: string;
  readonly computationTimeMs?: number;
}

/**
 * Request payload for route computation.
 */
export interface RouteRequest {
  readonly fromId: number;
  readonly toId: number;
  readonly algorithm: Algorithm;
}
