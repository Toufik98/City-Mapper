import { TestBed } from '@angular/core/testing';
import { provideStore } from '@ngrx/store';
import { provideHttpClient } from '@angular/common/http';
import { provideRouter } from '@angular/router';
import { TranslateModule } from '@ngx-translate/core';
import { App } from './app';
import { stationReducer, stationFeatureKey } from './store/station/station.reducer';
import { routeReducer, routeFeatureKey } from './store/route/route.reducer';
import { uiReducer, uiFeatureKey } from './store/ui/ui.reducer';

describe('App', () => {
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [App, TranslateModule.forRoot()],
      providers: [
        provideRouter([]),
        provideHttpClient(),
        provideStore({
          [stationFeatureKey]: stationReducer,
          [routeFeatureKey]: routeReducer,
          [uiFeatureKey]: uiReducer,
        }),
      ],
    }).compileComponents();
  });

  it('should create the app', () => {
    const fixture = TestBed.createComponent(App);
    const app = fixture.componentInstance;
    expect(app).toBeTruthy();
  });

  it('should have a toolbar', async () => {
    const fixture = TestBed.createComponent(App);
    fixture.detectChanges();
    await fixture.whenStable();
    const compiled = fixture.nativeElement as HTMLElement;
    expect(compiled.querySelector('mat-toolbar')).toBeTruthy();
  });
});
