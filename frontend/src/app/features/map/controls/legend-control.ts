import * as L from 'leaflet';
import { METRO_LINE_COLORS } from '../../../core/constants/metro-lines';

/**
 * Custom Leaflet control displaying the metro line color legend.
 */
export class LegendControl extends L.Control {
  private container: HTMLDivElement | null = null;

  constructor(options?: L.ControlOptions) {
    super({ position: 'bottomright', ...options });
  }

  override onAdd(_map: L.Map): HTMLElement {
    this.container = L.DomUtil.create('div', 'cm-legend-control');
    this.container.innerHTML = this.buildLegendHTML();

    // Prevent map interactions when clicking the legend
    L.DomEvent.disableClickPropagation(this.container);
    L.DomEvent.disableScrollPropagation(this.container);

    return this.container;
  }

  override onRemove(_map: L.Map): void {
    this.container = null;
  }

  private buildLegendHTML(): string {
    const lines = Object.entries(METRO_LINE_COLORS)
      .sort((a, b) => {
        const numA = parseFloat(a[0].replace('bis', '.5'));
        const numB = parseFloat(b[0].replace('bis', '.5'));
        return numA - numB;
      })
      .map(
        ([lineId, color]) => `
        <div class="cm-legend-item">
          <span class="cm-legend-color" style="background-color: ${color}"></span>
          <span class="cm-legend-label">Line ${lineId}</span>
        </div>
      `
      )
      .join('');

    return `
      <div class="cm-legend-header">Metro Lines</div>
      <div class="cm-legend-body">${lines}</div>
    `;
  }
}
