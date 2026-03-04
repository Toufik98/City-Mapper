import { createReducer, on } from '@ngrx/store';
import * as UIActions from './ui.actions';

export const uiFeatureKey = 'ui';

export interface UIState {
  language: string;
  accessibilityMode: boolean;
  darkMode: boolean;
  sidebarOpen: boolean;
  mapZoom: number;
}

export const initialUIState: UIState = {
  language: localStorage.getItem('cm-language') ?? 'fr',
  accessibilityMode: localStorage.getItem('cm-accessibility') === 'true',
  darkMode: localStorage.getItem('cm-dark-mode') === 'true',
  sidebarOpen: true,
  mapZoom: 12,
};

export const uiReducer = createReducer(
  initialUIState,

  on(UIActions.setLanguage, (state, { language }) => {
    localStorage.setItem('cm-language', language);
    return { ...state, language };
  }),

  on(UIActions.toggleAccessibility, (state) => {
    const newMode = !state.accessibilityMode;
    localStorage.setItem('cm-accessibility', String(newMode));
    return { ...state, accessibilityMode: newMode };
  }),

  on(UIActions.toggleSidebar, (state) => ({
    ...state,
    sidebarOpen: !state.sidebarOpen,
  })),

  on(UIActions.setSidebarOpen, (state, { open }) => ({
    ...state,
    sidebarOpen: open,
  })),

  on(UIActions.toggleDarkMode, (state) => {
    const newMode = !state.darkMode;
    localStorage.setItem('cm-dark-mode', String(newMode));
    return { ...state, darkMode: newMode };
  }),

  on(UIActions.setMapZoom, (state, { zoom }) => ({
    ...state,
    mapZoom: zoom,
  }))
);
