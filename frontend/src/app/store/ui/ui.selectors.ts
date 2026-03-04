import { createFeatureSelector, createSelector } from '@ngrx/store';
import { UIState, uiFeatureKey } from './ui.reducer';

export const selectUIState = createFeatureSelector<UIState>(uiFeatureKey);

export const selectLanguage = createSelector(
  selectUIState,
  (state) => state.language
);

export const selectIsAccessible = createSelector(
  selectUIState,
  (state) => state.accessibilityMode
);

export const selectSidebarOpen = createSelector(
  selectUIState,
  (state) => state.sidebarOpen
);

export const selectMapZoom = createSelector(
  selectUIState,
  (state) => state.mapZoom
);

export const selectDarkMode = createSelector(
  selectUIState,
  (state) => state.darkMode
);
