/**
 * Type of connection between two stations.
 */
export enum ConnectionType {
  RIDE = 'RIDE',
  TRANSFER = 'TRANSFER',
  WALK = 'WALK',
}

/**
 * A single segment of a computed route.
 * Represents travel between two consecutive stations.
 */
export interface RouteSegment {
  readonly fromStation: {
    readonly id: number;
    readonly name: string;
  };
  readonly toStation: {
    readonly id: number;
    readonly name: string;
  };
  readonly durationSeconds: number;
  readonly type: ConnectionType;
  readonly lineId: string | null;
  readonly lineName: string | null;
}
