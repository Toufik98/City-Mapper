import * as L from 'leaflet';
import { StationConnection } from '../../../core/models/connection.model';
import { AccessibilityService } from '../../../core/services/accessibility.service';

/**
 * Leaflet layer that renders metro line polylines connecting stations.
 * Draws actual graph edges representing tracks.
 */
export class LineLayer {
  private readonly layerGroup = L.layerGroup();
  private readonly linePolylines = new Map<string, L.Polyline>();

  constructor(private readonly accessibility: AccessibilityService) {}

  /**
   * Render polylines for all metro lines based on backend connections.
   */
  setConnections(connections: StationConnection[]): void {
    this.clear();

    // Group connections by line as multi-polylines
    const lineGroups = new Map<string, L.LatLngExpression[][]>();
    for (const conn of connections) {
      if (conn.fromLat && conn.fromLon && conn.toLat && conn.toLon) {
        const existing = lineGroups.get(conn.lineId) ?? [];
        existing.push([
          [conn.fromLat, conn.fromLon],
          [conn.toLat, conn.toLon]
        ]);
        lineGroups.set(conn.lineId, existing);
      }
    }

    // Draw a multipolyline per line avoiding spiderwebs
    for (const [lineId, segments] of lineGroups) {
      if (segments.length === 0) continue;

      const color = this.accessibility.getLineColor(lineId);
      const dashArray = this.accessibility.getDashPattern(lineId);

      const polyline = L.polyline(segments, {
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
