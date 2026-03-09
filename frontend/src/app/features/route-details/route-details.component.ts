import { Component, ChangeDetectionStrategy, inject } from '@angular/core';
import { Store } from '@ngrx/store';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatTooltipModule } from '@angular/material/tooltip';
import { TranslateModule } from '@ngx-translate/core';
import { SegmentCardComponent } from './segment-card.component';
import { TransferIndicatorComponent } from './transfer-indicator.component';
import { RouteSummaryComponent } from './route-summary.component';
import { LoadingSpinnerComponent } from '../../shared/components/loading-spinner.component';
import { ConnectionType } from '../../core/models/segment.model';
import { ShareRouteService } from '../../core/services/share-route.service';
import {
  selectCurrentRoute,
  selectRouteLoading,
  selectRouteError,
} from '../../store/route/route.selectors';
import { clearRoute } from '../../store/route/route.actions';

@Component({
  selector: 'app-route-details',
  standalone: true,
  imports: [
    MatCardModule,
    MatIconModule,
    MatButtonModule,
    MatTooltipModule,
    TranslateModule,
    SegmentCardComponent,
    TransferIndicatorComponent,
    RouteSummaryComponent,
    LoadingSpinnerComponent,
  ],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    @if (isLoading()) {
      <mat-card class="route-card" appearance="outlined">
        <app-loading-spinner />
      </mat-card>
    } @else if (error()) {
      <mat-card class="route-card error-card" appearance="outlined">
        <mat-card-content class="error-content">
          <mat-icon class="error-icon">error_outline</mat-icon>
          <p>{{ 'ROUTE.NO_ROUTE' | translate }}</p>
          <p class="error-detail">{{ error() }}</p>
        </mat-card-content>
      </mat-card>
    } @else if (route()) {
      <mat-card class="route-card" appearance="outlined">
        <div class="route-header">
          <h3>{{ 'ROUTE.TITLE' | translate }}</h3>
          <div class="route-header-actions">
            <button mat-icon-button (click)="onShare()"
                    [matTooltip]="'SHARE.SHARE_ROUTE' | translate"
                    aria-label="Share route">
              <mat-icon>share</mat-icon>
            </button>
            <button mat-icon-button (click)="onClose()" aria-label="Close">
              <mat-icon>close</mat-icon>
            </button>
          </div>
        </div>

        <app-route-summary [route]="route()" />

        <div class="segments-list">
          @for (segment of route()!.segments; track $index; let last = $last) {
            <app-segment-card [segment]="segment" />
            @if (!last && isTransfer($index)) {
              <app-transfer-indicator
                [durationSeconds]="route()!.segments[$index + 1]?.durationSeconds ?? 0"
                [isWalk]="route()!.segments[$index + 1]?.type === ConnectionType.WALK" />
            }
          }
        </div>
      </mat-card>
    }
  `,
  styles: [`
    .route-card {
      width: 380px;
      max-width: calc(100vw - 32px);
      max-height: calc(100vh - 280px);
      border-radius: var(--cm-panel-radius);
      box-shadow: var(--cm-glass-shadow);
      background: var(--cm-glass-bg);
      backdrop-filter: var(--cm-glass-blur);
      -webkit-backdrop-filter: var(--cm-glass-blur);
      border: 1px solid var(--cm-glass-border);
      overflow-y: auto;
    }

    .route-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 16px 16px 0;

      h3 {
        font-size: 18px;
        font-weight: 700;
        margin: 0;
        background: var(--cm-gradient-text);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
      }
    }

    .route-header-actions {
      display: flex;
      gap: 4px;
    }

    .segments-list {
      padding: 8px 16px 16px;
      display: flex;
      flex-direction: column;
    }

    .error-card {
      text-align: center;
    }

    .error-content {
      padding: 24px;
    }

    .error-icon {
      font-size: 48px;
      width: 48px;
      height: 48px;
      color: var(--cm-error);
      margin-bottom: 12px;
    }

    .error-detail {
      font-size: 13px;
      color: var(--cm-on-surface-variant);
      margin-top: 4px;
    }

    @media (max-width: 768px) {
      .route-card {
        width: 100%;
        max-height: 50vh;
        border-radius: var(--cm-radius-lg) var(--cm-radius-lg) 0 0;
      }
    }
  `],
})
export class RouteDetailsComponent {
  private readonly store = inject(Store);
  private readonly shareService = inject(ShareRouteService);

  readonly route = this.store.selectSignal(selectCurrentRoute);
  readonly isLoading = this.store.selectSignal(selectRouteLoading);
  readonly error = this.store.selectSignal(selectRouteError);
  readonly ConnectionType = ConnectionType;

  /**
   * Whether a transfer indicator should be shown after the given segment index.
   */
  isTransfer(index: number): boolean {
    const segments = this.route()?.segments;
    if (!segments || index >= segments.length - 1) return false;

    const current = segments[index];
    const next = segments[index + 1];

    return (
      current.type !== next.type ||
      current.lineId !== next.lineId
    );
  }

  onClose(): void {
    this.store.dispatch(clearRoute());
  }

  onShare(): void {
    this.shareService.shareCurrentRoute();
  }
}
