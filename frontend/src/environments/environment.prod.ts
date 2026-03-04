export const environment = {
  production: true,
  apiUrl: 'https://api.citymapper.local',
  defaultLanguage: 'fr',
  cacheSize: 500,
  map: {
    center: { lat: 48.8566, lng: 2.3522 } as const,
    defaultZoom: 12,
    minZoom: 10,
    maxZoom: 18,
    tileUrl: 'https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png',
    tileAttribution: '&copy; <a href="https://carto.com/">CARTO</a> &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a>',
  },
};
