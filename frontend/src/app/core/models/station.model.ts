/**
 * Represents a metro station in the RATP network.
 * Maps to the backend Station struct.
 */
export interface Station {
  readonly id: number;
  readonly name: string;
  readonly lineId: string;
  readonly lineName: string;
  readonly address: string;
  readonly latitude: number | null;
  readonly longitude: number | null;
  readonly lines: string[];
}

/**
 * Lightweight station reference for search results / autocomplete.
 */
export interface StationSummary {
  readonly id: number;
  readonly name: string;
  readonly lineId: string;
  readonly lineName: string;
}
