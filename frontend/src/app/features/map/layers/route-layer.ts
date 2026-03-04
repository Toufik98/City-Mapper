import * as L from 'leaflet';
import { Route } from '../../../core/models/route.model';
import { ConnectionType, RouteSegment } from '../../../core/models/segment.model';
import { AccessibilityService } from '../../../core/services/accessibility.service';
import { Station } from '../../../core/models/station.model';

/**
 * Animated route overlay layer showing the computed path on the map.
 * Each segment is colored by its metro line, with transfer points highlighted.
 */
export class RouteLayer {
  private readonly layerGroup = L.layerGroup();
  private animationFrame: number | null = null;
  private movingMarker: L.CircleMarker | null = null;

  constructor(private readonly accessibility: AccessibilityService) {}

  /**
   * Display a computed route on the map with animated tracing.
   * @param route - The computed route result.
   * @param stationMap - Map of station ID → Station for coordinate lookup.
   */
  displayRoute(route: Route, stationMap: Map<number, Station>): void {
    this.clear();

    const allPoints: L.LatLng[] = [];

    for (const segment of route.segments) {
      const from = stationMap.get(segment.fromStation.id);
      const to = stationMap.get(segment.toStation.id);

      if (!from?.latitude || !from?.longitude || !to?.latitude || !to?.longitude) {
        continue;
      }

      const fromLatLng = L.latLng(from.latitude, from.longitude);
      const toLatLng = L.latLng(to.latitude, to.longitude);
      allPoints.push(fromLatLng, toLatLng);

      // Draw segment polyline
      const color = segment.lineId
        ? this.accessibility.getLineColor(segment.lineId)
        : '#888888';

      const segmentLine = L.polyline([fromLatLng, toLatLng], {
        color,
        weight: 6,
        opacity: 0.9,
        lineCap: 'round',
        lineJoin: 'round',
        dashArray: segment.type === ConnectionType.WALK ? '8 8' : undefined,
      });

      segmentLine.addTo(this.layerGroup);

      // Mark transfer points
      if (segment.type === ConnectionType.TRANSFER) {
        const transferMarker = L.circleMarker(fromLatLng, {
          radius: 8,
          fillColor: '#FFFFFF',
          color: '#333333',
          weight: 2,
          fillOpacity: 1,
        });

        transferMarker.bindTooltip('Transfer', { direction: 'top' });
        transferMarker.addTo(this.layerGroup);
      }
    }

    // Mark start and end points
    if (allPoints.length >= 2) {
      this.addEndpointMarker(allPoints[0], '#00C853', 'Departure');
      this.addEndpointMarker(allPoints[allPoints.length - 1], '#E63946', 'Arrival');
      this.startAnimation(allPoints);
    }
  }

  /**
   * Get the bounding box of the displayed route for map fitting.
   */
  getBounds(): L.LatLngBounds | null {
    const layers = this.layerGroup.getLayers();
    if (layers.length === 0) return null;

    const bounds = L.latLngBounds([]);
    layers.forEach((layer) => {
      if (layer instanceof L.Polyline) {
        bounds.extend(layer.getBounds());
      } else if (layer instanceof L.CircleMarker) {
        bounds.extend(layer.getLatLng());
      }
    });

    return bounds.isValid() ? bounds : null;
  }

  addTo(map: L.Map): void {
    this.layerGroup.addTo(map);
  }

  remove(): void {
    this.stopAnimation();
    this.layerGroup.remove();
  }

  clear(): void {
    this.stopAnimation();
    this.layerGroup.clearLayers();
  }

  private addEndpointMarker(latlng: L.LatLng, color: string, label: string): void {
    const marker = L.circleMarker(latlng, {
      radius: 10,
      fillColor: color,
      color: '#FFFFFF',
      weight: 3,
      fillOpacity: 1,
    });

    marker.bindTooltip(label, { direction: 'top', permanent: true, offset: L.point(0, -12) });
    marker.addTo(this.layerGroup);
  }

  private startAnimation(points: L.LatLng[]): void {
    if (points.length < 2) return;

    this.movingMarker = L.circleMarker(points[0], {
      radius: 6,
      fillColor: '#003B8E',
      color: '#FFFFFF',
      weight: 2,
      fillOpacity: 1,
    });

    this.movingMarker.addTo(this.layerGroup);

    let currentIndex = 0;
    let progress = 0;
    const speed = 0.02; // Progress per frame (0 to 1 per segment)

    const animate = () => {
      if (currentIndex >= points.length - 1) {
        currentIndex = 0;
        progress = 0;
      }

      const from = points[currentIndex];
      const to = points[currentIndex + 1];

      const lat = from.lat + (to.lat - from.lat) * progress;
      const lng = from.lng + (to.lng - from.lng) * progress;

      this.movingMarker?.setLatLng([lat, lng]);

      progress += speed;
      if (progress >= 1) {
        progress = 0;
        currentIndex++;
      }

      this.animationFrame = requestAnimationFrame(animate);
    };

    this.animationFrame = requestAnimationFrame(animate);
  }

  private stopAnimation(): void {
    if (this.animationFrame !== null) {
      cancelAnimationFrame(this.animationFrame);
      this.animationFrame = null;
    }
    this.movingMarker = null;
  }
}
