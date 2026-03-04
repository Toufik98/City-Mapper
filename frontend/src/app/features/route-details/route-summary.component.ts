import { Component, Input, ChangeDetectionStrategy } from '@angular/core';
import { MatChipsModule } from '@angular/material/chips';
import { MatIconModule } from '@angular/material/icon';
import { TranslateModule } from '@ngx-translate/core';
import { Route } from '../../core/models/route.model';
import { DurationPipe } from '../../shared/pipes/duration.pipe';

/**
 * Summary header for a computed route showing total duration, transfers, and algorithm.
 */
@Component({
  selector: 'app-route-summary',
  standalone: true,
  imports: [MatChipsModule, MatIconModule, TranslateModule, DurationPipe],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    @if (route) {
      <div class="route-summary">
        <div class="summary-main">
          <mat-icon class="duration-icon">schedule</mat-icon>
          <span class="total-duration">{{ route.totalDurationSeconds | duration }}</span>
        </div>
        <div class="summary-details">
          <mat-chip-set>
            <mat-chip>
              <mat-icon matChipAvatar>swap_horiz</mat-icon>
              {{ route.totalTransfers }} {{ 'ROUTE.TRANSFERS' | translate }}
            </mat-chip>
            <mat-chip>
              <mat-icon matChipAvatar>timeline</mat-icon>
              {{ route.segments.length }} {{ 'ROUTE.STATIONS' | translate }}
            </mat-chip>
          </mat-chip-set>
        </div>
        @if (route.computationTimeMs) {
          <div class="computation-time">
            {{ 'ROUTE.COMPUTATION_TIME' | translate }} {{ route.computationTimeMs }}{{ 'ROUTE.MS' | translate }}
            · {{ route.algorithm }}
          </div>
        }
      </div>
    }
  `,
  styles: [`
    .route-summary {
      padding: 16px;
      border-bottom: 1px solid var(--cm-outline-variant);
    }

    .summary-main {
      display: flex;
      align-items: center;
      gap: 8px;
      margin-bottom: 8px;
    }

    .duration-icon {
      color: var(--cm-primary);
    }

    .total-duration {
      font-size: 24px;
      font-weight: 700;
      color: var(--cm-on-surface);
    }

    .summary-details {
      margin-bottom: 8px;
    }

    .computation-time {
      font-size: 11px;
      color: var(--cm-on-surface-variant);
    }
  `],
})
export class RouteSummaryComponent {
  @Input() route: Route | null = null;
}
