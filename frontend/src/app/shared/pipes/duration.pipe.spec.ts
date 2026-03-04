import { DurationPipe } from './duration.pipe';

describe('DurationPipe', () => {
  let pipe: DurationPipe;

  beforeEach(() => {
    pipe = new DurationPipe();
  });

  describe('full format', () => {
    it('should return empty string for null', () => {
      expect(pipe.transform(null)).toBe('');
    });

    it('should return empty string for undefined', () => {
      expect(pipe.transform(undefined)).toBe('');
    });

    it('should return empty string for negative values', () => {
      expect(pipe.transform(-5)).toBe('');
    });

    it('should format 0 seconds', () => {
      expect(pipe.transform(0)).toBe('0 s');
    });

    it('should format seconds only', () => {
      expect(pipe.transform(45)).toBe('45 s');
    });

    it('should format minutes and seconds', () => {
      expect(pipe.transform(125)).toBe('2 min 5 s');
    });

    it('should format exact minutes', () => {
      expect(pipe.transform(300)).toBe('5 min');
    });

    it('should format hours and minutes', () => {
      expect(pipe.transform(3660)).toBe('1 h 1 min');
    });

    it('should format hours with zero minutes', () => {
      expect(pipe.transform(3600)).toBe('1 h 0 min');
    });

    it('should not show seconds when hours > 0', () => {
      expect(pipe.transform(3665)).toBe('1 h 1 min');
    });
  });

  describe('compact format', () => {
    it('should format minutes compactly', () => {
      expect(pipe.transform(300, 'compact')).toBe('5 min');
    });

    it('should format hours compactly with padded minutes', () => {
      expect(pipe.transform(3660, 'compact')).toBe('1h01');
    });

    it('should format hours with zero minutes compactly', () => {
      expect(pipe.transform(3600, 'compact')).toBe('1h00');
    });
  });
});
