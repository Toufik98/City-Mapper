import { Directive, ElementRef, EventEmitter, HostListener, Output } from '@angular/core';

/**
 * Directive that emits an event when a click occurs outside the host element.
 * Useful for closing dropdowns, panels, etc.
 *
 * Usage: `<div (clickOutside)="onClose()">...</div>`
 */
@Directive({
  selector: '[clickOutside]',
  standalone: true,
})
export class ClickOutsideDirective {
  @Output() clickOutside = new EventEmitter<void>();

  constructor(private readonly elementRef: ElementRef<HTMLElement>) {}

  @HostListener('document:click', ['$event.target'])
  onDocumentClick(target: EventTarget | null): void {
    if (target instanceof Node && !this.elementRef.nativeElement.contains(target)) {
      this.clickOutside.emit();
    }
  }
}
