import { Pipe, PipeTransform } from '@angular/core';

/**
 * Formats a station name for display.
 * Trims whitespace and normalizes casing.
 */
@Pipe({
  name: 'stationName',
  standalone: true,
})
export class StationNamePipe implements PipeTransform {
  transform(name: string | null | undefined): string {
    if (!name) return '';
    return name
      .trim()
      .replace(/\s+/g, ' ')
      .replace(/\b\w/g, (char) => char.toUpperCase());
  }
}
