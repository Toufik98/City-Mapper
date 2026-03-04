import { Component, Output, EventEmitter, ChangeDetectionStrategy } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatTooltipModule } from '@angular/material/tooltip';
import { TranslateModule } from '@ngx-translate/core';

/**
 * Animated swap button to interchange departure and arrival stations.
 */
@Component({
  selector: 'app-swap-button',
  standalone: true,
  imports: [MatIconModule, MatButtonModule, MatTooltipModule, TranslateModule],
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `
    <button
      mat-icon-button
      [matTooltip]="'SEARCH.SWAP' | translate"
      (click)="onSwap()"
      class="swap-button"
      [attr.aria-label]="'SEARCH.SWAP' | translate">
      <mat-icon>swap_vert</mat-icon>
    </button>
  `,
  styles: [`
    .swap-button {
      transition: transform var(--cm-transition-normal);
    }

    .swap-button:hover {
      transform: rotate(180deg);
    }

    .swap-button:active {
      transform: rotate(180deg) scale(0.9);
    }
  `],
})
export class SwapButtonComponent {
  @Output() swap = new EventEmitter<void>();

  onSwap(): void {
    this.swap.emit();
  }
}
