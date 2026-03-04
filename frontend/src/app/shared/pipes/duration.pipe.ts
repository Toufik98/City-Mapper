import { Pipe, PipeTransform } from '@angular/core';

/**
 * Converts seconds into a human-readable duration string.
 * Examples: 125 → "2 min 5 s", 3600 → "1 h 0 min"
 */
@Pipe({
  name: 'duration',
  standalone: true,
})
export class DurationPipe implements PipeTransform {
  transform(seconds: number | null | undefined, format: 'full' | 'compact' = 'full'): string {
    if (seconds == null || seconds < 0) return '';

    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;

    if (format === 'compact') {
      if (hours > 0) return `${hours}h${minutes.toString().padStart(2, '0')}`;
      return `${minutes} min`;
    }

    const parts: string[] = [];
    if (hours > 0) parts.push(`${hours} h`);
    if (minutes > 0 || hours > 0) parts.push(`${minutes} min`);
    if (secs > 0 && hours === 0) parts.push(`${secs} s`);

    return parts.join(' ') || '0 s';
  }
}
