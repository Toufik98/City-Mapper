import { Component, Input, ChangeDetectionStrategy } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { DurationPipe } from '../../shared/pipes/duration.pipe';

/**
 * Visual indicator showing a transfer/walk between two segments.
 */
@Component({
  selector: 'app-transfer-indicator',
  standalone: true,
  imports: [MatIconModule, DurationPipe],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <div class="transfer-indicator">
      <div class="transfer-line"></div>
      <div class="transfer-content">
        <mat-icon class="transfer-icon">{{ isWalk ? 'directions_walk' : 'transfer_within_a_station' }}</mat-icon>
        @if (durationSeconds > 0) {
          <span class="transfer-duration">{{ durationSeconds | duration:'compact' }}</span>
        }
      </div>
      <div class="transfer-line"></div>
    </div>
  `,
  styles: [`
    .transfer-indicator {
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 4px 0;
    }

    .transfer-line {
      width: 2px;
      height: 8px;
      background: var(--cm-outline-variant);
      border-style: dashed;
    }

    .transfer-content {
      display: flex;
      align-items: center;
      gap: 4px;
      padding: 2px 8px;
      background: var(--cm-surface-variant);
      border-radius: var(--cm-radius-full);
      font-size: 11px;
      color: var(--cm-on-surface-variant);
    }

    .transfer-icon {
      font-size: 16px;
      width: 16px;
      height: 16px;
      color: var(--cm-tertiary);
    }

    .transfer-duration {
      font-weight: 500;
    }
  `],
})
export class TransferIndicatorComponent {
  @Input() durationSeconds = 0;
  @Input() isWalk = false;
}
