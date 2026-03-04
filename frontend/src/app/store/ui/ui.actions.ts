import { createAction, props } from '@ngrx/store';

// Language
export const setLanguage = createAction(
  '[UI] Set Language',
  props<{ language: string }>()
);

// Accessibility
export const toggleAccessibility = createAction('[UI] Toggle Accessibility');

// Dark Mode
export const toggleDarkMode = createAction('[UI] Toggle Dark Mode');

// Sidebar
export const toggleSidebar = createAction('[UI] Toggle Sidebar');
export const setSidebarOpen = createAction(
  '[UI] Set Sidebar Open',
  props<{ open: boolean }>()
);

// Map
export const setMapZoom = createAction(
  '[UI] Set Map Zoom',
  props<{ zoom: number }>()
);
