import { RecentSearchService, RecentSearch } from './recent-search.service';

describe('RecentSearchService', () => {
  let service: RecentSearchService;
  const STORAGE_KEY = 'cm-recent-searches';

  beforeEach(() => {
    service = new RecentSearchService();
    localStorage.clear();
  });

  afterEach(() => {
    localStorage.clear();
  });

  it('should return empty array when no searches exist', () => {
    expect(service.getAll()).toEqual([]);
  });

  it('should add a search', () => {
    service.add(1, 'Châtelet', 2, 'Nation');
    const results = service.getAll();
    expect(results.length).toBe(1);
    expect(results[0].departureId).toBe(1);
    expect(results[0].departureName).toBe('Châtelet');
    expect(results[0].arrivalId).toBe(2);
    expect(results[0].arrivalName).toBe('Nation');
  });

  it('should add most recent first', () => {
    service.add(1, 'A', 2, 'B');
    service.add(3, 'C', 4, 'D');
    const results = service.getAll();
    expect(results[0].departureId).toBe(3);
    expect(results[1].departureId).toBe(1);
  });

  it('should deduplicate same departure/arrival pair', () => {
    service.add(1, 'A', 2, 'B');
    service.add(3, 'C', 4, 'D');
    service.add(1, 'A', 2, 'B');
    const results = service.getAll();
    expect(results.length).toBe(2);
    expect(results[0].departureId).toBe(1); // Most recent
  });

  it('should cap at 10 entries', () => {
    for (let i = 0; i < 15; i++) {
      service.add(i, `Station ${i}`, i + 100, `Station ${i + 100}`);
    }
    expect(service.getAll().length).toBe(10);
  });

  it('should clear all searches', () => {
    service.add(1, 'A', 2, 'B');
    service.clear();
    expect(service.getAll()).toEqual([]);
  });

  it('should handle corrupted localStorage gracefully', () => {
    localStorage.setItem(STORAGE_KEY, 'not-json');
    expect(service.getAll()).toEqual([]);
  });
});
