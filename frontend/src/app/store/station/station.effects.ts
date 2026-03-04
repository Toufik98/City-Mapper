import { Injectable, inject } from '@angular/core';
import { Actions, createEffect, ofType } from '@ngrx/effects';
import { of } from 'rxjs';
import { catchError, debounceTime, distinctUntilChanged, map, switchMap } from 'rxjs/operators';
import { StationService } from '../../core/services/station.service';
import * as StationActions from './station.actions';

@Injectable()
export class StationEffects {
  private readonly actions$ = inject(Actions);
  private readonly stationService = inject(StationService);

  /**
   * Load all stations on app init.
   */
  readonly loadStations$ = createEffect(() =>
    this.actions$.pipe(
      ofType(StationActions.loadStations),
      switchMap(() =>
        this.stationService.getAll().pipe(
          map((stations) => StationActions.loadStationsSuccess({ stations })),
          catchError((error) =>
            of(StationActions.loadStationsFailure({ error: error.message }))
          )
        )
      )
    )
  );

  /**
   * Search stations with debounce for autocomplete.
   */
  readonly searchStations$ = createEffect(() =>
    this.actions$.pipe(
      ofType(StationActions.searchStations),
      debounceTime(300),
      distinctUntilChanged((prev, curr) => prev.query === curr.query),
      switchMap(({ query }) => {
        if (query.length < 2) {
          return of(StationActions.searchStationsSuccess({ results: [] }));
        }
        return this.stationService.search(query).pipe(
          map((results) => StationActions.searchStationsSuccess({ results })),
          catchError((error) =>
            of(StationActions.searchStationsFailure({ error: error.message }))
          )
        );
      })
    )
  );
}
