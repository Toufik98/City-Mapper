import {
  Component,
  Output,
  EventEmitter,
  OnDestroy,
  ChangeDetectionStrategy,
  inject,
  signal,
} from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatTooltipModule } from '@angular/material/tooltip';
import { TranslateModule, TranslateService } from '@ngx-translate/core';
import { Subject, takeUntil } from 'rxjs';
import { SpeechService } from '../../core/services/speech.service';

/**
 * Microphone button that listens for voice input and emits the recognized text.
 */
@Component({
  selector: 'app-voice-input',
  standalone: true,
  imports: [MatIconModule, MatButtonModule, MatTooltipModule, TranslateModule],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    @if (isSupported) {
      <button
        mat-icon-button
        [matTooltip]="(isListening() ? 'SEARCH.VOICE_LISTENING' : 'SEARCH.VOICE_INPUT') | translate"
        (click)="toggle()"
        [class.listening]="isListening()"
        [attr.aria-label]="'SEARCH.VOICE_INPUT' | translate">
        <mat-icon>{{ isListening() ? 'mic' : 'mic_none' }}</mat-icon>
      </button>
    }
  `,
  styles: [`
    button {
      transition: color var(--cm-transition-fast);
    }

    button.listening {
      color: var(--cm-error);
      animation: pulse 1.5s ease-in-out infinite;
    }

    @keyframes pulse {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.5; }
    }
  `],
})
export class VoiceInputComponent implements OnDestroy {
  @Output() transcript = new EventEmitter<string>();

  private readonly speech = inject(SpeechService);
  private readonly translate = inject(TranslateService);
  private readonly destroy$ = new Subject<void>();

  readonly isListening = signal(false);
  readonly isSupported = this.speech.isSupported();

  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
    this.speech.stopListening();
  }

  toggle(): void {
    if (this.isListening()) {
      this.speech.stopListening();
      this.isListening.set(false);
    } else {
      const lang = this.translate.currentLang === 'fr' ? 'fr-FR' : 'en-US';

      this.speech
        .startListening(lang)
        .pipe(takeUntil(this.destroy$))
        .subscribe({
          next: (text) => {
            this.transcript.emit(text);
            this.isListening.set(false);
          },
          error: () => this.isListening.set(false),
        });

      this.speech.isListening$
        .pipe(takeUntil(this.destroy$))
        .subscribe((listening) => this.isListening.set(listening));
    }
  }
}
