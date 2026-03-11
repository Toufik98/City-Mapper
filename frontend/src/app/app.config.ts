import { ApplicationConfig, provideBrowserGlobalErrorListeners, importProvidersFrom } from '@angular/core';
import { provideRouter } from '@angular/router';
import { provideHttpClient, withInterceptors } from '@angular/common/http';
import { provideAnimations } from '@angular/platform-browser/animations';
import { provideStore } from '@ngrx/store';
import { provideEffects } from '@ngrx/effects';
import { provideStoreDevtools } from '@ngrx/store-devtools';
import { TranslateModule, provideTranslateService } from '@ngx-translate/core';
import { provideTranslateHttpLoader } from '@ngx-translate/http-loader';

import { routes } from './app.routes';
import { errorInterceptor } from './core/interceptors/error.interceptor';
import { stationReducer, stationFeatureKey } from './store/station/station.reducer';
import { routeReducer, routeFeatureKey } from './store/route/route.reducer';
import { uiReducer, uiFeatureKey } from './store/ui/ui.reducer';
import { StationEffects } from './store/station/station.effects';
import { RouteEffects } from './store/route/route.effects';
import { environment } from '../environments/environment';

export const appConfig: ApplicationConfig = {
  providers: [
    provideBrowserGlobalErrorListeners(),
    provideRouter(routes),
    provideHttpClient(withInterceptors([errorInterceptor])),
    provideAnimations(),

    // NgRx Store
    provideStore({
      [stationFeatureKey]: stationReducer,
      [routeFeatureKey]: routeReducer,
      [uiFeatureKey]: uiReducer,
    }),
    provideEffects([StationEffects, RouteEffects]),
    provideStoreDevtools({
      maxAge: 25,
      logOnly: environment.production,
    }),

    // i18n — order matters: provideTranslateService registers the NoOp loader,
    // then provideTranslateHttpLoader overrides it with the HTTP loader.
    importProvidersFrom(TranslateModule),
    provideTranslateService({
      defaultLanguage: environment.defaultLanguage,
    }),
    provideTranslateHttpLoader({ prefix: './assets/i18n/', suffix: '.json' }),
  ],
};
