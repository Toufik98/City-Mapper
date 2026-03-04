import { Component, Input, ChangeDetectionStrategy, inject } from '@angular/core';
import { AccessibilityService } from '../../core/services/accessibility.service';
import {
  METRO_LINE_COLORS,
  METRO_LINE_TEXT_COLORS,
  DEFAULT_LINE_COLOR,
  DEFAULT_TEXT_COLOR,
} from '../../core/constants/metro-lines';

/**
 * Colored circle badge displaying a metro line number.
 * Adapts colors based on accessibility mode.
 *
 * Usage: `<app-metro-line-badge lineId="4" />`
 */
@Component({
  selector: 'app-metro-line-badge',
  standalone: true,
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <span
      class="line-badge"
      [style.background-color]="backgroundColor"
      [style.color]="textColor"
      [attr.aria-label]="'Line ' + lineId"
      role="img">
      {{ displayId }}
    </span>
  `,
  styles: [`
    .line-badge {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      width: 24px;
      height: 24px;
      border-radius: 50%;
      font-size: 11px;
      font-weight: 700;
      line-height: 1;
      flex-shrink: 0;
      user-select: none;
    }

    :host(.large) .line-badge {
      width: 32px;
      height: 32px;
      font-size: 14px;
    }
  `],
})
export class MetroLineBadgeComponent {
  private readonly accessibility = inject(AccessibilityService);

  @Input({ required: true }) lineId!: string;
  @Input() size: 'small' | 'large' = 'small';

  get displayId(): string {
    return this.lineId.replace('bis', 'b');
  }

  get backgroundColor(): string {
    return this.accessibility.getLineColor(this.lineId);
  }

  get textColor(): string {
    return METRO_LINE_TEXT_COLORS[this.lineId] ?? DEFAULT_TEXT_COLOR;
  }
}
