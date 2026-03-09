import { Component, OnInit, OnDestroy, inject, Renderer2 } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { Store } from '@ngrx/store';
import { Subject, takeUntil } from 'rxjs';
import { MatToolbarModule } from '@angular/material/toolbar';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatSidenavModule } from '@angular/material/sidenav';
import { MatTooltipModule } from '@angular/material/tooltip';
import { TranslateModule, TranslateService } from '@ngx-translate/core';
import { SearchPanelComponent } from './features/search/search-panel.component';
import { RouteDetailsComponent } from './features/route-details/route-details.component';
import { SettingsPanelComponent } from './features/settings/settings-panel.component';
import { loadStations } from './store/station/station.actions';
import { selectCurrentRoute } from './store/route/route.selectors';
import { selectSidebarOpen, selectDarkMode, selectLanguage } from './store/ui/ui.selectors';
import { toggleSidebar, toggleDarkMode } from './store/ui/ui.actions';
import { KeyboardShortcutService } from './core/services/keyboard-shortcut.service';

@Component({
  selector: 'app-root',
  imports: [
    RouterOutlet,
    MatToolbarModule,
    MatIconModule,
    MatButtonModule,
    MatSidenavModule,
    MatTooltipModule,
    TranslateModule,
    SearchPanelComponent,
    RouteDetailsComponent,
    SettingsPanelComponent,
  ],
  templateUrl: './app.html',
  styleUrl: './app.scss',
})
export class App implements OnInit, OnDestroy {
  private readonly store = inject(Store);
  private readonly renderer = inject(Renderer2);
  private readonly keyboard = inject(KeyboardShortcutService);
  private readonly destroy$ = new Subject<void>();

  readonly hasRoute = this.store.selectSignal(selectCurrentRoute);
  readonly sidebarOpen = this.store.selectSignal(selectSidebarOpen);
  readonly darkMode = this.store.selectSignal(selectDarkMode);
  readonly currentLanguage = this.store.selectSignal(selectLanguage);
  
  showSettings = false;

  constructor(private translate: TranslateService) {}

  ngOnInit(): void {
    // Initialize default language
    this.translate.setDefaultLang('en');
    
    // Subscribe to language changes
    this.store.select(selectLanguage).pipe(takeUntil(this.destroy$)).subscribe(lang => {
      this.translate.use(lang);
    });

    this.store.dispatch(loadStations());
    this.keyboard.init();

    // Apply dark mode class to <html> reactively
    this.store
      .select(selectDarkMode)
      .pipe(takeUntil(this.destroy$))
      .subscribe((isDark) => {
        if (isDark) {
          this.renderer.addClass(document.documentElement, 'dark-theme');
        } else {
          this.renderer.removeClass(document.documentElement, 'dark-theme');
        }
      });
  }

  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
    this.keyboard.destroy();
  }

  toggleSettings(): void {
    this.showSettings = !this.showSettings;
  }

  toggleSidebar(): void {
    this.store.dispatch(toggleSidebar());
  }

  toggleDarkMode(): void {
    this.store.dispatch(toggleDarkMode());
  }
}
