import { Component, Input, ChangeDetectionStrategy, inject } from '@angular/core';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { TranslateModule } from '@ngx-translate/core';
import { RouteSegment, ConnectionType } from '../../core/models/segment.model';
import { MetroLineBadgeComponent } from '../../shared/components/metro-line-badge.component';
import { DurationPipe } from '../../shared/pipes/duration.pipe';
import { AccessibilityService } from '../../core/services/accessibility.service';

/**
 * Card displaying a single route segment (ride, transfer, or walk).
 */
@Component({
  selector: 'app-segment-card',
  standalone: true,
  imports: [MatCardModule, MatIconModule, TranslateModule, MetroLineBadgeComponent, DurationPipe],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <div class="segment-card" [style.border-left-color]="borderColor">
      <div class="segment-header">
        @if (segment.type === ConnectionType.RIDE && segment.lineId) {
          <app-metro-line-badge [lineId]="segment.lineId" />
          <span class="segment-type">{{ segment.lineName || 'Line ' + segment.lineId }}</span>
        } @else if (segment.type === ConnectionType.TRANSFER) {
          <mat-icon class="transfer-icon">transfer_within_a_station</mat-icon>
          <span class="segment-type">{{ 'ROUTE.SEGMENT.TRANSFER' | translate:{ line: segment.lineId } }}</span>
        } @else {
          <mat-icon class="walk-icon">directions_walk</mat-icon>
          <span class="segment-type">{{ 'ROUTE.SEGMENT.WALK' | translate:{ station: segment.toStation.name } }}</span>
        }
        <span class="segment-duration">{{ segment.durationSeconds | duration:'compact' }}</span>
      </div>
      <div class="segment-body">
        <span class="station-name from">{{ segment.fromStation.name }}</span>
        <mat-icon class="arrow-icon">arrow_forward</mat-icon>
        <span class="station-name to">{{ segment.toStation.name }}</span>
      </div>
    </div>
  `,
  styles: [`
    .segment-card {
      border-left: 4px solid var(--cm-outline-variant);
      padding: 12px 16px;
      margin: 4px 0;
      border-radius: 0 var(--cm-radius-sm) var(--cm-radius-sm) 0;
      background: var(--cm-surface);
      transition: background var(--cm-transition-fast);
    }

    .segment-card:hover {
      background: var(--cm-surface-variant);
    }

    .segment-header {
      display: flex;
      align-items: center;
      gap: 8px;
      margin-bottom: 4px;
    }

    .segment-type {
      font-weight: 500;
      font-size: 14px;
      flex: 1;
    }

    .segment-duration {
      font-size: 13px;
      font-weight: 600;
      color: var(--cm-primary);
    }

    .segment-body {
      display: flex;
      align-items: center;
      gap: 6px;
      font-size: 13px;
      color: var(--cm-on-surface-variant);
    }

    .station-name {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }

    .arrow-icon {
      font-size: 16px;
      width: 16px;
      height: 16px;
      flex-shrink: 0;
    }

    .transfer-icon {
      color: var(--cm-tertiary);
    }

    .walk-icon {
      color: var(--cm-secondary);
    }
  `],
})
export class SegmentCardComponent {
  private readonly accessibility = inject(AccessibilityService);

  @Input({ required: true }) segment!: RouteSegment;

  readonly ConnectionType = ConnectionType;

  get borderColor(): string {
    if (this.segment.lineId) {
      return this.accessibility.getLineColor(this.segment.lineId);
    }
    return '#888888';
  }
}
