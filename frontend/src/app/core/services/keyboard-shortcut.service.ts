import { Injectable, inject, NgZone } from '@angular/core';
import { Store } from '@ngrx/store';
import { fromEvent, filter, Subject, takeUntil } from 'rxjs';
import { DOCUMENT } from '@angular/common';
import { toggleSidebar, toggleDarkMode } from '../../store/ui/ui.actions';
import { clearRoute } from '../../store/route/route.actions';

/**
 * Global keyboard shortcuts:
 * - `/` or `Ctrl+K` → Focus search input
 * - `Escape`        → Close panels / clear route
 * - `Ctrl+D`        → Toggle dark mode
 * - `Ctrl+Shift+A`  → Toggle sidebar
 */
@Injectable({ providedIn: 'root' })
export class KeyboardShortcutService {
  private readonly store = inject(Store);
  private readonly zone = inject(NgZone);
  private readonly document = inject(DOCUMENT);
  private readonly destroy$ = new Subject<void>();

  /**
   * Initialize keyboard listener. Call once from root component.
   */
  init(): void {
    this.zone.runOutsideAngular(() => {
      fromEvent<KeyboardEvent>(this.document, 'keydown')
        .pipe(
          filter((e) => !this.isInputFocused(e)),
          takeUntil(this.destroy$)
        )
        .subscribe((event) => this.handleKeydown(event));
    });
  }

  destroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
  }

  private handleKeydown(event: KeyboardEvent): void {
    // `/` or Ctrl+K → Focus departure search
    if (event.key === '/' || (event.ctrlKey && event.key === 'k')) {
      event.preventDefault();
      this.zone.run(() => this.focusSearch());
      return;
    }

    // Escape → Close settings / clear route
    if (event.key === 'Escape') {
      this.zone.run(() => this.store.dispatch(clearRoute()));
      return;
    }

    // Ctrl+D → Toggle dark mode
    if (event.ctrlKey && event.key === 'd') {
      event.preventDefault();
      this.zone.run(() => this.store.dispatch(toggleDarkMode()));
      return;
    }

    // Ctrl+Shift+A → Toggle sidebar
    if (event.ctrlKey && event.shiftKey && event.key === 'A') {
      event.preventDefault();
      this.zone.run(() => this.store.dispatch(toggleSidebar()));
      return;
    }
  }

  private focusSearch(): void {
    const input = this.document.querySelector<HTMLInputElement>(
      'app-station-autocomplete input'
    );
    input?.focus();
    input?.select();
  }

  private isInputFocused(event: KeyboardEvent): boolean {
    const target = event.target as HTMLElement;
    const tagName = target.tagName.toLowerCase();
    // Allow "/" shortcut to work only outside input/textarea
    if (event.key === '/' || (event.ctrlKey && event.key === 'k')) {
      return tagName === 'textarea';
    }
    return false;
  }
}
