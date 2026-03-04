import { Component, ChangeDetectionStrategy } from '@angular/core';
import { RouterLink } from '@angular/router';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { TranslateModule } from '@ngx-translate/core';

/**
 * 404 page with a metro-themed design and a link back to the map.
 */
@Component({
  selector: 'app-not-found',
  standalone: true,
  imports: [RouterLink, MatButtonModule, MatIconModule, TranslateModule],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <div class="not-found-container">
      <div class="not-found-content">
        <mat-icon class="not-found-icon">wrong_location</mat-icon>
        <h1 class="not-found-title">404</h1>
        <p class="not-found-message">{{ 'ERRORS.PAGE_NOT_FOUND' | translate }}</p>
        <p class="not-found-hint">{{ 'ERRORS.PAGE_NOT_FOUND_HINT' | translate }}</p>
        <a mat-flat-button color="primary" routerLink="/" class="home-btn">
          <mat-icon>map</mat-icon>
          {{ 'ERRORS.BACK_TO_MAP' | translate }}
        </a>
      </div>
    </div>
  `,
  styles: [`
    .not-found-container {
      display: flex;
      align-items: center;
      justify-content: center;
      height: 100%;
      width: 100%;
      background: linear-gradient(
        135deg,
        var(--cm-primary-dark) 0%,
        var(--cm-primary) 50%,
        var(--cm-primary-light) 100%
      );
    }

    .not-found-content {
      text-align: center;
      color: white;
      padding: var(--cm-spacing-xl);
    }

    .not-found-icon {
      font-size: 80px;
      width: 80px;
      height: 80px;
      opacity: 0.8;
    }

    .not-found-title {
      font-size: 96px;
      font-weight: 800;
      margin: var(--cm-spacing-md) 0;
      letter-spacing: -2px;
      line-height: 1;
    }

    .not-found-message {
      font-size: 20px;
      font-weight: 500;
      margin-bottom: var(--cm-spacing-sm);
    }

    .not-found-hint {
      font-size: 14px;
      opacity: 0.8;
      margin-bottom: var(--cm-spacing-xl);
    }

    .home-btn {
      font-size: 16px;
      padding: 12px 32px;
      border-radius: var(--cm-radius-full);
    }
  `],
})
export class NotFoundComponent {}
