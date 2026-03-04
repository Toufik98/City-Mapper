import { Injectable, NgZone, inject } from '@angular/core';
import { Observable, Subject } from 'rxjs';

/**
 * Web Speech API wrapper for voice-based station search.
 * Gracefully handles browser incompatibility.
 */
@Injectable({ providedIn: 'root' })
export class SpeechService {
  private readonly zone = inject(NgZone);
  private recognition: SpeechRecognition | null = null;
  private readonly transcript$ = new Subject<string>();
  private readonly listening$ = new Subject<boolean>();

  /**
   * Whether the Web Speech API is supported in the current browser.
   */
  isSupported(): boolean {
    return 'SpeechRecognition' in window || 'webkitSpeechRecognition' in window;
  }

  /**
   * Start listening for speech input.
   * @param lang - BCP 47 language tag (default: 'fr-FR')
   * @returns Observable that emits recognized transcript strings.
   */
  startListening(lang = 'fr-FR'): Observable<string> {
    if (!this.isSupported()) {
      this.transcript$.error(new Error('Speech recognition not supported'));
      return this.transcript$.asObservable();
    }

    this.stopListening();

    const SpeechRecognitionCtor =
      (window as unknown as { SpeechRecognition: typeof SpeechRecognition }).SpeechRecognition ??
      (window as unknown as { webkitSpeechRecognition: typeof SpeechRecognition }).webkitSpeechRecognition;

    this.recognition = new SpeechRecognitionCtor();
    this.recognition.lang = lang;
    this.recognition.interimResults = false;
    this.recognition.maxAlternatives = 1;
    this.recognition.continuous = false;

    this.recognition.onresult = (event: SpeechRecognitionEvent) => {
      this.zone.run(() => {
        const result = event.results[0]?.[0]?.transcript ?? '';
        this.transcript$.next(result.trim());
      });
    };

    this.recognition.onstart = () => {
      this.zone.run(() => this.listening$.next(true));
    };

    this.recognition.onend = () => {
      this.zone.run(() => this.listening$.next(false));
    };

    this.recognition.onerror = (event: SpeechRecognitionErrorEvent) => {
      this.zone.run(() => {
        this.listening$.next(false);
        if (event.error !== 'no-speech' && event.error !== 'aborted') {
          this.transcript$.error(new Error(`Speech recognition error: ${event.error}`));
        }
      });
    };

    this.recognition.start();
    return this.transcript$.asObservable();
  }

  /**
   * Stop any active listening session.
   */
  stopListening(): void {
    if (this.recognition) {
      this.recognition.abort();
      this.recognition = null;
    }
  }

  /**
   * Observable that emits `true` when listening and `false` when stopped.
   */
  get isListening$(): Observable<boolean> {
    return this.listening$.asObservable();
  }
}
