import { Component, ChangeDetectionStrategy, inject } from '@angular/core';
import { Store } from '@ngrx/store';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonToggleModule } from '@angular/material/button-toggle';
import { TranslateModule, TranslateService } from '@ngx-translate/core';
import { setLanguage } from '../../store/ui/ui.actions';
import { selectLanguage } from '../../store/ui/ui.selectors';

/**
 * Language selector toggle buttons (FR / EN).
 */
@Component({
  selector: 'app-language-selector',
  standalone: true,
  imports: [MatButtonModule, MatIconModule, MatButtonToggleModule, TranslateModule],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <div class="language-section">
      <label class="section-label">{{ 'SETTINGS.LANGUAGE' | translate }}</label>
      <mat-button-toggle-group
        [value]="currentLang()"
        (change)="onLanguageChange($event.value)"
        aria-label="Language selection">
        <mat-button-toggle value="fr">FR</mat-button-toggle>
        <mat-button-toggle value="en">EN</mat-button-toggle>
      </mat-button-toggle-group>
    </div>
  `,
  styles: [`
    .language-section {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 8px 0;
    }

    .section-label {
      font-weight: 500;
      font-size: 14px;
    }
  `],
})
export class LanguageSelectorComponent {
  private readonly store = inject(Store);
  private readonly translate = inject(TranslateService);

  readonly currentLang = this.store.selectSignal(selectLanguage);

  onLanguageChange(language: string): void {
    this.store.dispatch(setLanguage({ language }));
    this.translate.use(language);
  }
}
