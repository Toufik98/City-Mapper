import * as L from 'leaflet';
import { Station } from '../../../core/models/station.model';
import { AccessibilityService } from '../../../core/services/accessibility.service';

/**
 * Leaflet layer that renders metro line polylines connecting stations.
 * Groups stations by line and draws colored paths.
 */
export class LineLayer {
  private readonly layerGroup = L.layerGroup();
  private readonly linePolylines = new Map<string, L.Polyline>();

  constructor(private readonly accessibility: AccessibilityService) {}

  /**
   * Render polylines for all metro lines based on station positions.
   * Requires stations to have valid coordinates and be sorted by position on the line.
   */
  setStations(stations: Station[]): void {
    this.clear();

    // Group stations by line
    const lineGroups = new Map<string, Station[]>();
    for (const station of stations) {
      if (station.latitude && station.longitude) {
        const existing = lineGroups.get(station.lineId) ?? [];
        existing.push(station);
        lineGroups.set(station.lineId, existing);
      }
    }

    // Draw a polyline per line
    for (const [lineId, lineStations] of lineGroups) {
      if (lineStations.length < 2) continue;

      const coords: L.LatLngExpression[] = lineStations.map((s) => [
        s.latitude!,
        s.longitude!,
      ]);

      const color = this.accessibility.getLineColor(lineId);
      const dashArray = this.accessibility.getDashPattern(lineId);

      const polyline = L.polyline(coords, {
        color,
        weight: 3,
        opacity: 0.7,
        dashArray: dashArray || undefined,
        lineCap: 'round',
        lineJoin: 'round',
      });

      polyline.addTo(this.layerGroup);
      this.linePolylines.set(lineId, polyline);
    }
  }

  /**
   * Toggle visibility of a specific metro line.
   */
  toggleLine(lineId: string, visible: boolean): void {
    const polyline = this.linePolylines.get(lineId);
    if (polyline) {
      if (visible) {
        this.layerGroup.addLayer(polyline);
      } else {
        this.layerGroup.removeLayer(polyline);
      }
    }
  }

  addTo(map: L.Map): void {
    this.layerGroup.addTo(map);
  }

  remove(): void {
    this.layerGroup.remove();
  }

  private clear(): void {
    this.layerGroup.clearLayers();
    this.linePolylines.clear();
  }
}
