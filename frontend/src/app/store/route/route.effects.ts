import { Injectable, inject } from '@angular/core';
import { Actions, createEffect, ofType } from '@ngrx/effects';
import { Store } from '@ngrx/store';
import { of } from 'rxjs';
import { catchError, map, switchMap, withLatestFrom } from 'rxjs/operators';
import { RouteService } from '../../core/services/route.service';
import * as RouteActions from './route.actions';
import { selectDeparture, selectArrival, selectAlgorithm } from './route.selectors';

@Injectable()
export class RouteEffects {
  private readonly actions$ = inject(Actions);
  private readonly store = inject(Store);
  private readonly routeService = inject(RouteService);

  /**
   * Compute route when action dispatched.
   * Reads departure, arrival, and algorithm from store.
   */
  readonly computeRoute$ = createEffect(() =>
    this.actions$.pipe(
      ofType(RouteActions.computeRoute),
      withLatestFrom(
        this.store.select(selectDeparture),
        this.store.select(selectArrival),
        this.store.select(selectAlgorithm)
      ),
      switchMap(([, departure, arrival, algorithm]) => {
        if (!departure || !arrival) {
          return of(
            RouteActions.computeRouteFailure({
              error: 'Please select both departure and arrival stations.',
            })
          );
        }

        if (departure.id === arrival.id) {
          return of(
            RouteActions.computeRouteFailure({
              error: 'Departure and arrival stations must be different.',
            })
          );
        }

        return this.routeService
          .computeRoute(departure.id, arrival.id, algorithm)
          .pipe(
            map((route) => RouteActions.computeRouteSuccess({ route })),
            catchError((error) =>
              of(RouteActions.computeRouteFailure({ error: error.message }))
            )
          );
      })
    )
  );
}
