export const environment = {
  production: false,
  apiUrl: 'http://localhost:8080',
  defaultLanguage: 'fr',
  cacheSize: 100,
  map: {
    center: { lat: 48.8566, lng: 2.3522 } as const,
    defaultZoom: 12,
    minZoom: 10,
    maxZoom: 18,
    tileUrl: 'https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png',
    tileAttribution: '&copy; <a href="https://carto.com/">CARTO</a> &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a>',
  },
};
