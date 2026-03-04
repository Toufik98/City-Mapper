import { uiReducer, initialUIState, UIState } from './ui.reducer';
import * as UIActions from './ui.actions';

describe('UI Reducer', () => {
  beforeEach(() => {
    localStorage.clear();
  });

  it('should return the initial state', () => {
    const result = uiReducer(undefined, { type: 'unknown' });
    expect(result.language).toBeDefined();
    expect(result.sidebarOpen).toBe(true);
    expect(result.mapZoom).toBe(12);
  });

  it('should set language', () => {
    const result = uiReducer(
      initialUIState,
      UIActions.setLanguage({ language: 'en' })
    );
    expect(result.language).toBe('en');
    expect(localStorage.getItem('cm-language')).toBe('en');
  });

  it('should toggle accessibility mode', () => {
    const result = uiReducer(
      { ...initialUIState, accessibilityMode: false },
      UIActions.toggleAccessibility()
    );
    expect(result.accessibilityMode).toBe(true);
    expect(localStorage.getItem('cm-accessibility')).toBe('true');
  });

  it('should toggle accessibility mode off', () => {
    const result = uiReducer(
      { ...initialUIState, accessibilityMode: true },
      UIActions.toggleAccessibility()
    );
    expect(result.accessibilityMode).toBe(false);
    expect(localStorage.getItem('cm-accessibility')).toBe('false');
  });

  it('should toggle dark mode', () => {
    const result = uiReducer(
      { ...initialUIState, darkMode: false },
      UIActions.toggleDarkMode()
    );
    expect(result.darkMode).toBe(true);
    expect(localStorage.getItem('cm-dark-mode')).toBe('true');
  });

  it('should toggle sidebar', () => {
    const result = uiReducer(
      { ...initialUIState, sidebarOpen: true },
      UIActions.toggleSidebar()
    );
    expect(result.sidebarOpen).toBe(false);
  });

  it('should set sidebar open', () => {
    const result = uiReducer(
      { ...initialUIState, sidebarOpen: false },
      UIActions.setSidebarOpen({ open: true })
    );
    expect(result.sidebarOpen).toBe(true);
  });

  it('should set map zoom', () => {
    const result = uiReducer(
      initialUIState,
      UIActions.setMapZoom({ zoom: 15 })
    );
    expect(result.mapZoom).toBe(15);
  });
});
