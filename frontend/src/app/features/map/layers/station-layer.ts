import * as L from 'leaflet';
import { Station } from '../../../core/models/station.model';
import { AccessibilityService } from '../../../core/services/accessibility.service';
import { METRO_LINE_TEXT_COLORS } from '../../../core/constants/metro-lines';

/**
 * Leaflet layer that renders all metro stations as circle markers.
 * Colors match the official RATP palette (or accessible palette).
 */
export class StationLayer {
  private readonly layerGroup = L.layerGroup();
  private readonly markers = new Map<number, L.CircleMarker>();

  constructor(
    private readonly accessibility: AccessibilityService,
    private readonly onStationClick: (station: Station) => void
  ) {}

  /**
   * Add all stations to the layer.
   */
  setStations(stations: Station[]): void {
    this.clear();
    for (const station of stations) {
      if (station.latitude && station.longitude) {
        this.addStationMarker(station);
      }
    }
  }

  /**
   * Highlight a specific station (e.g., selected departure/arrival).
   */
  highlightStation(stationId: number): void {
    const marker = this.markers.get(stationId);
    if (marker) {
      marker.setRadius(10);
      marker.setStyle({ weight: 3, fillOpacity: 1 });
      marker.bringToFront();
    }
  }

  /**
   * Reset all markers to default style.
   */
  resetHighlights(): void {
    this.markers.forEach((marker) => {
      marker.setRadius(5);
      marker.setStyle({ weight: 1.5, fillOpacity: 0.8 });
    });
  }

  /**
   * Add this layer to a Leaflet map.
   */
  addTo(map: L.Map): void {
    this.layerGroup.addTo(map);
  }

  /**
   * Remove this layer from the map.
   */
  remove(): void {
    this.layerGroup.remove();
  }

  private addStationMarker(station: Station): void {
    const color = this.accessibility.getLineColor(station.lineId);

    const marker = L.circleMarker([station.latitude!, station.longitude!], {
      radius: 5,
      fillColor: color,
      color: '#FFFFFF',
      weight: 1.5,
      opacity: 1,
      fillOpacity: 0.8,
    });

    marker.bindTooltip(
      `<strong>${station.name}</strong><br>Line ${station.lineId}`,
      { direction: 'top', offset: L.point(0, -8) }
    );

    marker.on('click', () => this.onStationClick(station));

    marker.addTo(this.layerGroup);
    this.markers.set(station.id, marker);
  }

  private clear(): void {
    this.layerGroup.clearLayers();
    this.markers.clear();
  }
}
