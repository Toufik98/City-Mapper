import { Component, Input, ChangeDetectionStrategy } from '@angular/core';
import { MetroLineBadgeComponent } from './metro-line-badge.component';

/**
 * Chip displaying a station name alongside its metro line badge.
 *
 * Usage: `<app-station-chip name="Châtelet" lineId="1" />`
 */
@Component({
  selector: 'app-station-chip',
  standalone: true,
  imports: [MetroLineBadgeComponent],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <span class="station-chip">
      <app-metro-line-badge [lineId]="lineId" />
      <span class="station-name">{{ name }}</span>
    </span>
  `,
  styles: [`
    .station-chip {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 4px 12px 4px 4px;
      background: var(--cm-surface-variant);
      border-radius: var(--cm-radius-full);
      font-size: 14px;
      font-weight: 500;
      color: var(--cm-on-surface);
    }

    .station-name {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
      max-width: 200px;
    }
  `],
})
export class StationChipComponent {
  @Input({ required: true }) name!: string;
  @Input({ required: true }) lineId!: string;
}
