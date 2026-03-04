import { Component, ChangeDetectionStrategy, inject } from '@angular/core';
import { Store } from '@ngrx/store';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
import { TranslateModule } from '@ngx-translate/core';
import { toggleAccessibility } from '../../store/ui/ui.actions';
import { selectIsAccessible } from '../../store/ui/ui.selectors';

/**
 * Toggle switch for colorblind-safe accessibility mode.
 */
@Component({
  selector: 'app-accessibility-toggle',
  standalone: true,
  imports: [MatSlideToggleModule, TranslateModule],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <div class="accessibility-section">
      <div class="toggle-row">
        <div class="toggle-info">
          <span class="toggle-label">{{ 'SETTINGS.COLORBLIND_MODE' | translate }}</span>
          <span class="toggle-description">{{ 'SETTINGS.COLORBLIND_DESCRIPTION' | translate }}</span>
        </div>
        <mat-slide-toggle
          [checked]="isAccessible()"
          (change)="onToggle()"
          [attr.aria-label]="'SETTINGS.COLORBLIND_MODE' | translate" />
      </div>
    </div>
  `,
  styles: [`
    .accessibility-section {
      padding: 8px 0;
    }

    .toggle-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 16px;
    }

    .toggle-info {
      display: flex;
      flex-direction: column;
    }

    .toggle-label {
      font-weight: 500;
      font-size: 14px;
    }

    .toggle-description {
      font-size: 12px;
      color: var(--cm-on-surface-variant);
      margin-top: 2px;
    }
  `],
})
export class AccessibilityToggleComponent {
  private readonly store = inject(Store);

  readonly isAccessible = this.store.selectSignal(selectIsAccessible);

  onToggle(): void {
    this.store.dispatch(toggleAccessibility());
  }
}
