import { Component, ChangeDetectionStrategy } from '@angular/core';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

/**
 * Centered loading spinner overlay.
 */
@Component({
  selector: 'app-loading-spinner',
  standalone: true,
  imports: [MatProgressSpinnerModule],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <div class="spinner-overlay" role="status" aria-label="Loading">
      <mat-spinner diameter="48" />
    </div>
  `,
  styles: [`
    .spinner-overlay {
      display: flex;
      align-items: center;
      justify-content: center;
      padding: var(--cm-spacing-lg);
    }
  `],
})
export class LoadingSpinnerComponent {}
