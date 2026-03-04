import { Injectable, inject } from '@angular/core';
import { Router } from '@angular/router';
import { Store } from '@ngrx/store';
import { DOCUMENT } from '@angular/common';
import { MatSnackBar } from '@angular/material/snack-bar';
import { TranslateService } from '@ngx-translate/core';
import { selectDeparture, selectArrival } from '../../store/route/route.selectors';
import { take } from 'rxjs';

/**
 * Generates shareable URLs for routes and handles deep-linking.
 * URL format: /route/:fromId/:toId
 */
@Injectable({ providedIn: 'root' })
export class ShareRouteService {
  private readonly store = inject(Store);
  private readonly router = inject(Router);
  private readonly document = inject(DOCUMENT);
  private readonly snackBar = inject(MatSnackBar);
  private readonly translate = inject(TranslateService);

  /**
   * Generate a shareable URL for the current departure/arrival selection.
   * Copies the URL to clipboard and shows a snackbar notification.
   */
  shareCurrentRoute(): void {
    this.store.select(selectDeparture).pipe(take(1)).subscribe((departure) => {
      this.store.select(selectArrival).pipe(take(1)).subscribe((arrival) => {
        if (!departure || !arrival) return;

        const url = `${this.document.location.origin}/route/${departure.id}/${arrival.id}`;
        navigator.clipboard.writeText(url).then(() => {
          const msg = this.translate.instant('SHARE.COPIED');
          this.snackBar.open(msg, '✓', { duration: 3000 });
        }).catch(() => {
          // Fallback: select text in a temporary input
          const input = this.document.createElement('input');
          input.value = url;
          this.document.body.appendChild(input);
          input.select();
          this.document.execCommand('copy');
          this.document.body.removeChild(input);
          const msg = this.translate.instant('SHARE.COPIED');
          this.snackBar.open(msg, '✓', { duration: 3000 });
        });
      });
    });
  }

  /**
   * Build a shareable URL string without copying.
   */
  buildUrl(fromId: number, toId: number): string {
    return `${this.document.location.origin}/route/${fromId}/${toId}`;
  }
}
