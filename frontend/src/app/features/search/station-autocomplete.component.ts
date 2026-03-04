import {
  Component,
  Input,
  Output,
  EventEmitter,
  OnInit,
  OnDestroy,
  ChangeDetectionStrategy,
  inject,
} from '@angular/core';
import { FormControl, ReactiveFormsModule } from '@angular/forms';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatAutocompleteModule } from '@angular/material/autocomplete';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { Subject, debounceTime, distinctUntilChanged, filter, takeUntil } from 'rxjs';
import { Store } from '@ngrx/store';
import { TranslateModule } from '@ngx-translate/core';
import { Station } from '../../core/models/station.model';
import { MetroLineBadgeComponent } from '../../shared/components/metro-line-badge.component';
import { searchStations, clearSearch } from '../../store/station/station.actions';
import { selectSearchResults, selectSearching } from '../../store/station/station.selectors';

@Component({
  selector: 'app-station-autocomplete',
  standalone: true,
  imports: [
    ReactiveFormsModule,
    MatFormFieldModule,
    MatInputModule,
    MatAutocompleteModule,
    MatIconModule,
    MatButtonModule,
    TranslateModule,
    MetroLineBadgeComponent,
  ],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <mat-form-field appearance="outline" class="station-field">
      <mat-label>{{ label | translate }}</mat-label>
      <mat-icon matPrefix>{{ icon }}</mat-icon>
      <input
        matInput
        [formControl]="searchControl"
        [matAutocomplete]="auto"
        [placeholder]="placeholder | translate"
        (focus)="onFocus()"
        autocomplete="off" />
      @if (searchControl.value) {
        <button
          mat-icon-button
          matSuffix
          (click)="onClear()"
          [attr.aria-label]="'SEARCH.CLEAR' | translate">
          <mat-icon>close</mat-icon>
        </button>
      }
      <mat-autocomplete
        #auto="matAutocomplete"
        [displayWith]="displayStation"
        (optionSelected)="onStationSelected($event.option.value)"
        class="station-autocomplete">
        @for (station of searchResults(); track station.id) {
          <mat-option [value]="station">
            <div class="option-content">
              <app-metro-line-badge [lineId]="station.lineId" />
              <span class="option-name">{{ station.name }}</span>
              <span class="option-line">{{ station.lineName }}</span>
            </div>
          </mat-option>
        }
        @if (searchControl.value && searchControl.value.length >= 2 && searchResults().length === 0 && !isSearching()) {
          <mat-option disabled>
            {{ 'SEARCH.NO_RESULTS' | translate }}
          </mat-option>
        }
      </mat-autocomplete>
    </mat-form-field>
  `,
  styles: [`
    .station-field {
      width: 100%;
    }

    .option-content {
      display: flex;
      align-items: center;
      gap: 8px;
      width: 100%;
    }

    .option-name {
      font-weight: 500;
      flex: 1;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }

    .option-line {
      font-size: 12px;
      color: var(--cm-on-surface-variant);
      flex-shrink: 0;
    }
  `],
})
export class StationAutocompleteComponent implements OnInit, OnDestroy {
  private readonly store = inject(Store);
  private readonly destroy$ = new Subject<void>();

  @Input({ required: true }) label!: string;
  @Input({ required: true }) placeholder!: string;
  @Input() icon = 'place';
  @Input() selectedStation: Station | null = null;

  @Output() stationSelected = new EventEmitter<Station>();
  @Output() stationCleared = new EventEmitter<void>();

  readonly searchControl = new FormControl('');

  readonly searchResults = this.store.selectSignal(selectSearchResults);
  readonly isSearching = this.store.selectSignal(selectSearching);

  ngOnInit(): void {
    // Debounced search
    this.searchControl.valueChanges
      .pipe(
        takeUntil(this.destroy$),
        debounceTime(300),
        distinctUntilChanged(),
        filter((value): value is string => typeof value === 'string')
      )
      .subscribe((query) => {
        if (query.length >= 2) {
          this.store.dispatch(searchStations({ query }));
        } else {
          this.store.dispatch(clearSearch());
        }
      });

    // Set initial value if station is pre-selected
    if (this.selectedStation) {
      this.searchControl.setValue(this.selectedStation.name, { emitEvent: false });
    }
  }

  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
  }

  onStationSelected(station: Station): void {
    this.stationSelected.emit(station);
    this.store.dispatch(clearSearch());
  }

  onClear(): void {
    this.searchControl.setValue('');
    this.stationCleared.emit();
    this.store.dispatch(clearSearch());
  }

  onFocus(): void {
    const value = this.searchControl.value;
    if (typeof value === 'string' && value.length >= 2) {
      this.store.dispatch(searchStations({ query: value }));
    }
  }

  /**
   * Display function for mat-autocomplete.
   */
  displayStation(station: Station | string | null): string {
    if (!station) return '';
    if (typeof station === 'string') return station;
    return station.name;
  }

  /**
   * Programmatically set the displayed value (e.g., after voice input or swap).
   */
  setValue(station: Station | null): void {
    this.searchControl.setValue(station?.name ?? '', { emitEvent: false });
  }
}
