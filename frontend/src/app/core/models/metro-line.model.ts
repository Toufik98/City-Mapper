/**
 * Represents a Paris Metro line with visual properties.
 */
export interface MetroLine {
  readonly id: string;
  readonly name: string;
  readonly color: string;
  readonly accessibleColor: string;
  readonly textColor: string;
}
