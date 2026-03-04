import { Component, ChangeDetectionStrategy, inject } from '@angular/core';
import { Store } from '@ngrx/store';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatDividerModule } from '@angular/material/divider';
import { MatRadioModule } from '@angular/material/radio';
import { FormsModule } from '@angular/forms';
import { TranslateModule } from '@ngx-translate/core';
import { LanguageSelectorComponent } from './language-selector.component';
import { AccessibilityToggleComponent } from './accessibility-toggle.component';
import { Algorithm } from '../../core/models/route.model';
import { setAlgorithm } from '../../store/route/route.actions';
import { selectAlgorithm } from '../../store/route/route.selectors';

@Component({
  selector: 'app-settings-panel',
  standalone: true,
  imports: [
    MatCardModule,
    MatIconModule,
    MatDividerModule,
    MatRadioModule,
    FormsModule,
    TranslateModule,
    LanguageSelectorComponent,
    AccessibilityToggleComponent,
  ],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <mat-card appearance="outlined" class="settings-card">
      <mat-card-header>
        <mat-card-title class="settings-title">
          <mat-icon>settings</mat-icon>
          {{ 'SETTINGS.TITLE' | translate }}
        </mat-card-title>
      </mat-card-header>

      <mat-card-content class="settings-content">
        <app-language-selector />

        <mat-divider />

        <div class="section">
          <label class="section-label">{{ 'SETTINGS.ACCESSIBILITY' | translate }}</label>
          <app-accessibility-toggle />
        </div>

        <mat-divider />

        <div class="section">
          <label class="section-label">{{ 'SETTINGS.ALGORITHM.TITLE' | translate }}</label>
          <mat-radio-group
            [ngModel]="currentAlgorithm()"
            (ngModelChange)="onAlgorithmChange($event)"
            class="algorithm-group">
            <mat-radio-button [value]="Algorithm.DIJKSTRA">
              {{ 'SETTINGS.ALGORITHM.DIJKSTRA' | translate }}
            </mat-radio-button>
            <mat-radio-button [value]="Algorithm.A_STAR">
              {{ 'SETTINGS.ALGORITHM.A_STAR' | translate }}
            </mat-radio-button>
          </mat-radio-group>
        </div>
      </mat-card-content>
    </mat-card>
  `,
  styles: [`
    .settings-card {
      border-radius: var(--cm-radius-lg);
      box-shadow: var(--cm-shadow-2);
    }

    .settings-title {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 16px;
    }

    .settings-content {
      display: flex;
      flex-direction: column;
      gap: 8px;
      padding-top: 16px;
    }

    .section {
      padding: 8px 0;
    }

    .section-label {
      display: block;
      font-weight: 600;
      font-size: 13px;
      color: var(--cm-on-surface-variant);
      text-transform: uppercase;
      letter-spacing: 0.5px;
      margin-bottom: 8px;
    }

    .algorithm-group {
      display: flex;
      flex-direction: column;
      gap: 4px;
    }
  `],
})
export class SettingsPanelComponent {
  private readonly store = inject(Store);

  readonly Algorithm = Algorithm;
  readonly currentAlgorithm = this.store.selectSignal(selectAlgorithm);

  onAlgorithmChange(algorithm: Algorithm): void {
    this.store.dispatch(setAlgorithm({ algorithm }));
  }
}
