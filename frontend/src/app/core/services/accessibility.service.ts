import { Injectable, inject, signal, computed } from '@angular/core';
import { Store } from '@ngrx/store';
import {
  METRO_LINE_COLORS,
  METRO_LINE_TEXT_COLORS,
  DEFAULT_LINE_COLOR,
  DEFAULT_TEXT_COLOR,
} from '../constants/metro-lines';
import { METRO_LINE_COLORS_ACCESSIBLE, METRO_LINE_DASH_PATTERNS } from '../constants/metro-lines-accessible';
import { selectIsAccessible } from '../../store/ui/ui.selectors';
import { toSignal } from '@angular/core/rxjs-interop';

/**
 * Manages accessibility features including colorblind mode.
 * Provides the correct color palette based on user preference.
 */
@Injectable({ providedIn: 'root' })
export class AccessibilityService {
  private readonly store = inject(Store);
  private readonly isAccessible = toSignal(this.store.select(selectIsAccessible), {
    initialValue: false,
  });

  /**
   * Get the display color for a metro line, respecting accessibility mode.
   */
  getLineColor(lineId: string): string {
    if (this.isAccessible()) {
      return METRO_LINE_COLORS_ACCESSIBLE[lineId] ?? DEFAULT_LINE_COLOR;
    }
    return METRO_LINE_COLORS[lineId] ?? DEFAULT_LINE_COLOR;
  }

  /**
   * Get the text color for optimal contrast on a line's background.
   */
  getTextColor(lineId: string): string {
    return METRO_LINE_TEXT_COLORS[lineId] ?? DEFAULT_TEXT_COLOR;
  }

  /**
   * Get the dash pattern for a line in accessibility mode.
   * Returns empty string (solid) when accessibility mode is off.
   */
  getDashPattern(lineId: string): string {
    if (this.isAccessible()) {
      return METRO_LINE_DASH_PATTERNS[lineId] ?? '';
    }
    return '';
  }

  /**
   * Whether accessibility mode is currently active.
   */
  get isAccessibilityMode(): boolean {
    return this.isAccessible();
  }
}
