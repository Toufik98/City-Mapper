import { Component, ChangeDetectionStrategy, inject, signal, Output, EventEmitter } from '@angular/core';
import { MatListModule } from '@angular/material/list';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatDividerModule } from '@angular/material/divider';
import { TranslateModule } from '@ngx-translate/core';
import { RecentSearchService, RecentSearch } from '../../core/services/recent-search.service';

/**
 * Displays recent route searches. Emits selected search for re-execution.
 */
@Component({
  selector: 'app-recent-searches',
  standalone: true,
  imports: [MatListModule, MatIconModule, MatButtonModule, MatDividerModule, TranslateModule],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    @if (recentSearches().length > 0) {
      <div class="recent-searches">
        <div class="recent-header">
          <span class="recent-title">{{ 'SEARCH.RECENT' | translate }}</span>
          <button mat-icon-button (click)="clearAll()" class="clear-btn"
                  [attr.aria-label]="'SEARCH.CLEAR_HISTORY' | translate">
            <mat-icon>delete_sweep</mat-icon>
          </button>
        </div>
        <mat-divider />
        <mat-nav-list>
          @for (search of recentSearches(); track search.timestamp) {
            <mat-list-item (click)="select(search)">
              <mat-icon matListItemIcon>history</mat-icon>
              <span matListItemTitle class="search-route">
                {{ search.departureName }}
              </span>
              <span matListItemLine class="search-destination">
                → {{ search.arrivalName }}
              </span>
            </mat-list-item>
          }
        </mat-nav-list>
      </div>
    }
  `,
  styles: [`
    .recent-searches {
      margin-top: var(--cm-spacing-sm);
    }

    .recent-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 0 var(--cm-spacing-md);
    }

    .recent-title {
      font-size: 13px;
      font-weight: 600;
      color: var(--cm-on-surface-variant);
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }

    .clear-btn {
      color: var(--cm-on-surface-variant);
    }

    .search-route {
      font-weight: 500;
    }

    .search-destination {
      color: var(--cm-on-surface-variant);
      font-size: 13px;
    }

    mat-list-item {
      cursor: pointer;
    }
  `],
})
export class RecentSearchesComponent {
  @Output() searchSelected = new EventEmitter<RecentSearch>();

  private readonly recentService = inject(RecentSearchService);
  readonly recentSearches = signal<RecentSearch[]>(this.recentService.getAll());

  select(search: RecentSearch): void {
    this.searchSelected.emit(search);
  }

  clearAll(): void {
    this.recentService.clear();
    this.recentSearches.set([]);
  }

  /** Refresh the list (call after a new search is added) */
  refresh(): void {
    this.recentSearches.set(this.recentService.getAll());
  }
}
