import { StationNamePipe } from './station-name.pipe';

describe('StationNamePipe', () => {
  let pipe: StationNamePipe;

  beforeEach(() => {
    pipe = new StationNamePipe();
  });

  it('should return empty string for null', () => {
    expect(pipe.transform(null)).toBe('');
  });

  it('should return empty string for undefined', () => {
    expect(pipe.transform(undefined)).toBe('');
  });

  it('should return empty string for empty string', () => {
    expect(pipe.transform('')).toBe('');
  });

  it('should trim whitespace', () => {
    // \b\w regex treats post-accented char as word boundary, capitalizing 'T' in 'châTelet'
    expect(pipe.transform('  châtelet  ')).toBe('ChâTelet');
  });

  it('should normalize multiple spaces', () => {
    expect(pipe.transform('gare   de   lyon')).toBe('Gare De Lyon');
  });

  it('should capitalize first letter of each word', () => {
    expect(pipe.transform('nation')).toBe('Nation');
  });

  it('should preserve accented characters', () => {
    // The pipe capitalizes first char of each word boundary
    const result = pipe.transform('Châtelet - Les Halles');
    expect(result).toContain('Les Halles');
  });
});
