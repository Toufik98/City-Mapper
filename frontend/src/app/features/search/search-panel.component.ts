import {
  Component,
  ChangeDetectionStrategy,
  ViewChild,
  inject,
} from '@angular/core';
import { Store } from '@ngrx/store';
import { MatCardModule } from '@angular/material/card';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressBarModule } from '@angular/material/progress-bar';
import { TranslateModule } from '@ngx-translate/core';
import { StationAutocompleteComponent } from './station-autocomplete.component';
import { VoiceInputComponent } from './voice-input.component';
import { SwapButtonComponent } from './swap-button.component';
import { RecentSearchesComponent } from './recent-searches.component';
import { Station } from '../../core/models/station.model';
import { RecentSearchService, RecentSearch } from '../../core/services/recent-search.service';
import {
  setDeparture,
  setArrival,
  swapStations,
  computeRoute,
  clearDeparture,
  clearArrival,
} from '../../store/route/route.actions';
import {
  selectDeparture,
  selectArrival,
  selectRouteLoading,
  selectHasValidSelection,
} from '../../store/route/route.selectors';

@Component({
  selector: 'app-search-panel',
  standalone: true,
  imports: [
    MatCardModule,
    MatButtonModule,
    MatIconModule,
    MatProgressBarModule,
    TranslateModule,
    StationAutocompleteComponent,
    VoiceInputComponent,
    SwapButtonComponent,
    RecentSearchesComponent,
  ],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <mat-card class="search-card" appearance="outlined">
      <mat-card-header>
        <mat-card-title class="search-title">
          <mat-icon class="title-icon">directions_subway</mat-icon>
          {{ 'APP.TITLE' | translate }}
        </mat-card-title>
        <mat-card-subtitle>{{ 'APP.SUBTITLE' | translate }}</mat-card-subtitle>
      </mat-card-header>

      <mat-card-content class="search-content">
        <!-- Departure -->
        <div class="input-row">
          <app-station-autocomplete
            #departureInput
            label="SEARCH.DEPARTURE"
            placeholder="SEARCH.DEPARTURE_PLACEHOLDER"
            icon="trip_origin"
            [selectedStation]="departure()"
            (stationSelected)="onDepartureSelected($event)"
            (stationCleared)="onDepartureCleared()" />
          <app-voice-input (transcript)="onDepartureVoice($event)" />
        </div>

        <!-- Swap button -->
        <div class="swap-row">
          <app-swap-button (swap)="onSwap()" />
        </div>

        <!-- Arrival -->
        <div class="input-row">
          <app-station-autocomplete
            #arrivalInput
            label="SEARCH.ARRIVAL"
            placeholder="SEARCH.ARRIVAL_PLACEHOLDER"
            icon="location_on"
            [selectedStation]="arrival()"
            (stationSelected)="onArrivalSelected($event)"
            (stationCleared)="onArrivalCleared()" />
          <app-voice-input (transcript)="onArrivalVoice($event)" />
        </div>

        <!-- Loading bar -->
        @if (isLoading()) {
          <mat-progress-bar mode="indeterminate" class="route-progress" />
        }
      </mat-card-content>

      <mat-card-actions align="end">
        <button
          mat-flat-button
          color="primary"
          [disabled]="!canCompute() || isLoading()"
          (click)="onFindRoute()"
          class="find-route-btn">
          <mat-icon>search</mat-icon>
          {{ (isLoading() ? 'SEARCH.COMPUTING' : 'SEARCH.FIND_ROUTE') | translate }}
        </button>
      </mat-card-actions>

      <!-- Recent searches -->
      <app-recent-searches
        #recentSearches
        (searchSelected)="onRecentSearchSelected($event)" />
    </mat-card>
  `,
  styles: [`
    .search-card {
      width: 380px;
      max-width: calc(100vw - 32px);
      border-radius: var(--cm-radius-lg);
      box-shadow: var(--cm-shadow-3);
    }

    .search-title {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 18px;
    }

    .title-icon {
      color: var(--cm-primary);
    }

    .search-content {
      display: flex;
      flex-direction: column;
      gap: 4px;
      padding-top: 16px;
    }

    .input-row {
      display: flex;
      align-items: center;
      gap: 4px;
    }

    .input-row app-station-autocomplete {
      flex: 1;
    }

    .swap-row {
      display: flex;
      justify-content: center;
      margin: -8px 0;
    }

    .route-progress {
      margin-top: 8px;
    }

    .find-route-btn {
      width: 100%;
      height: 48px;
      font-size: 16px;
      font-weight: 600;
    }

    @media (max-width: 768px) {
      .search-card {
        width: 100%;
        border-radius: 0 0 var(--cm-radius-lg) var(--cm-radius-lg);
        box-shadow: var(--cm-shadow-2);
      }
    }
  `],
})
export class SearchPanelComponent {
  @ViewChild('departureInput') departureInput!: StationAutocompleteComponent;
  @ViewChild('arrivalInput') arrivalInput!: StationAutocompleteComponent;
  @ViewChild('recentSearches') recentSearchesComponent!: RecentSearchesComponent;

  private readonly store = inject(Store);
  private readonly recentSearchService = inject(RecentSearchService);

  readonly departure = this.store.selectSignal(selectDeparture);
  readonly arrival = this.store.selectSignal(selectArrival);
  readonly isLoading = this.store.selectSignal(selectRouteLoading);
  readonly canCompute = this.store.selectSignal(selectHasValidSelection);

  onDepartureSelected(station: Station): void {
    this.store.dispatch(setDeparture({ station }));
  }

  onArrivalSelected(station: Station): void {
    this.store.dispatch(setArrival({ station }));
  }

  onDepartureCleared(): void {
    this.store.dispatch(clearDeparture());
  }

  onArrivalCleared(): void {
    this.store.dispatch(clearArrival());
  }

  onSwap(): void {
    this.store.dispatch(swapStations());
    // Update input displays after swap
    setTimeout(() => {
      this.departureInput?.setValue(this.departure());
      this.arrivalInput?.setValue(this.arrival());
    });
  }

  onDepartureVoice(text: string): void {
    this.departureInput?.searchControl.setValue(text);
  }

  onArrivalVoice(text: string): void {
    this.arrivalInput?.searchControl.setValue(text);
  }

  onFindRoute(): void {
    const dep = this.departure();
    const arr = this.arrival();
    if (dep && arr) {
      this.recentSearchService.add(dep.id, dep.name, arr.id, arr.name);
      this.recentSearchesComponent?.refresh();
    }
    this.store.dispatch(computeRoute());
  }

  onRecentSearchSelected(search: RecentSearch): void {
    // Set departure and arrival from the recent search summary
    const departure: Station = { id: search.departureId, name: search.departureName } as Station;
    const arrival: Station = { id: search.arrivalId, name: search.arrivalName } as Station;
    this.store.dispatch(setDeparture({ station: departure }));
    this.store.dispatch(setArrival({ station: arrival }));
    this.departureInput?.setValue(departure);
    this.arrivalInput?.setValue(arrival);

    // Auto-compute the route
    setTimeout(() => this.store.dispatch(computeRoute()));
  }
}
