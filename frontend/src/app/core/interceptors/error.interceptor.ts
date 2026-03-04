import { HttpErrorResponse, HttpInterceptorFn } from '@angular/common/http';
import { inject } from '@angular/core';
import { MatSnackBar } from '@angular/material/snack-bar';
import { TranslateService } from '@ngx-translate/core';
import { catchError, throwError } from 'rxjs';

/**
 * Functional HTTP interceptor that catches errors and displays
 * user-friendly snackbar notifications.
 */
export const errorInterceptor: HttpInterceptorFn = (req, next) => {
  const snackBar = inject(MatSnackBar);
  const translate = inject(TranslateService);

  return next(req).pipe(
    catchError((error: HttpErrorResponse) => {
      let messageKey: string;

      if (error.status === 0) {
        messageKey = 'ERRORS.NETWORK';
      } else if (error.status === 404) {
        messageKey = 'ERRORS.NOT_FOUND';
      } else if (error.status >= 500) {
        messageKey = 'ERRORS.SERVER';
      } else {
        messageKey = 'ERRORS.UNKNOWN';
      }

      const message = translate.instant(messageKey);
      snackBar.open(message, '✕', {
        duration: 5000,
        panelClass: 'error-snackbar',
        horizontalPosition: 'center',
        verticalPosition: 'bottom',
      });

      return throwError(() => error);
    })
  );
};
