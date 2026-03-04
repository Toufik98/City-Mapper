import { Injectable } from '@angular/core';

/**
 * A recent route search entry persisted in localStorage.
 */
export interface RecentSearch {
  readonly departureId: number;
  readonly departureName: string;
  readonly arrivalId: number;
  readonly arrivalName: string;
  readonly timestamp: number;
}

const STORAGE_KEY = 'cm-recent-searches';
const MAX_RECENT = 10;

/**
 * Service for storing and retrieving recent route searches.
 * Uses localStorage with a max capacity of 10 entries (FIFO eviction).
 */
@Injectable({ providedIn: 'root' })
export class RecentSearchService {
  /**
   * Get all recent searches, most recent first.
   */
  getAll(): RecentSearch[] {
    try {
      const raw = localStorage.getItem(STORAGE_KEY);
      if (!raw) return [];
      return JSON.parse(raw) as RecentSearch[];
    } catch {
      return [];
    }
  }

  /**
   * Add a new search to the history. Deduplicates and caps at MAX_RECENT.
   */
  add(departureId: number, departureName: string, arrivalId: number, arrivalName: string): void {
    const searches = this.getAll().filter(
      (s) => !(s.departureId === departureId && s.arrivalId === arrivalId)
    );

    searches.unshift({
      departureId,
      departureName,
      arrivalId,
      arrivalName,
      timestamp: Date.now(),
    });

    if (searches.length > MAX_RECENT) {
      searches.length = MAX_RECENT;
    }

    try {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(searches));
    } catch {
      // Storage full — silently fail
    }
  }

  /**
   * Clear all recent searches.
   */
  clear(): void {
    localStorage.removeItem(STORAGE_KEY);
  }
}
