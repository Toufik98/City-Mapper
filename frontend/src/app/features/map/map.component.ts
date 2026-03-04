import {
  Component,
  OnInit,
  OnDestroy,
  ChangeDetectionStrategy,
  ElementRef,
  ViewChild,
  AfterViewInit,
  inject,
} from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { Store } from '@ngrx/store';
import { Subject, takeUntil, combineLatest, filter, take, switchMap } from 'rxjs';
import * as L from 'leaflet';
import { environment } from '../../../environments/environment';
import { AccessibilityService } from '../../core/services/accessibility.service';
import { StationService } from '../../core/services/station.service';
import { StationLayer } from './layers/station-layer';
import { LineLayer } from './layers/line-layer';
import { RouteLayer } from './layers/route-layer';
import { LegendControl } from './controls/legend-control';
import { selectAllStations } from '../../store/station/station.selectors';
import { selectCurrentRoute } from '../../store/route/route.selectors';
import { setDeparture, setArrival, computeRoute } from '../../store/route/route.actions';
import { setMapZoom } from '../../store/ui/ui.actions';
import { selectDeparture } from '../../store/route/route.selectors';
import { Station } from '../../core/models/station.model';

@Component({
  selector: 'app-map',
  standalone: true,
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `<div #mapContainer class="map-container"></div>`,
  styles: [`
    :host {
      display: block;
      width: 100%;
      height: 100%;
      position: relative;
    }

    .map-container {
      width: 100%;
      height: 100%;
    }

    :host ::ng-deep .cm-legend-control {
      background: white;
      border-radius: var(--cm-radius-md);
      box-shadow: var(--cm-shadow-2);
      padding: 12px;
      max-height: 300px;
      overflow-y: auto;
      font-size: 12px;
    }

    :host ::ng-deep .cm-legend-header {
      font-weight: 600;
      font-size: 13px;
      margin-bottom: 8px;
      color: var(--cm-on-surface);
    }

    :host ::ng-deep .cm-legend-item {
      display: flex;
      align-items: center;
      gap: 8px;
      padding: 2px 0;
    }

    :host ::ng-deep .cm-legend-color {
      width: 16px;
      height: 16px;
      border-radius: 50%;
      flex-shrink: 0;
    }

    :host ::ng-deep .cm-legend-label {
      color: var(--cm-on-surface-variant);
    }
  `],
})
export class MapComponent implements OnInit, AfterViewInit, OnDestroy {
  @ViewChild('mapContainer', { static: true }) mapContainer!: ElementRef<HTMLDivElement>;

  private readonly store = inject(Store);
  private readonly route = inject(ActivatedRoute);
  private readonly accessibility = inject(AccessibilityService);
  private readonly stationService = inject(StationService);
  private readonly destroy$ = new Subject<void>();

  private map!: L.Map;
  private stationLayer!: StationLayer;
  private lineLayer!: LineLayer;
  private routeLayer!: RouteLayer;
  private stationMap = new Map<number, Station>();
  private isSelectingDeparture = true;

  ngOnInit(): void {
    // Fix Leaflet's default icon path issue with bundlers
    delete (L.Icon.Default.prototype as any)['_getIconUrl'];
    L.Icon.Default.mergeOptions({
      iconRetinaUrl: 'assets/marker-icon-2x.png',
      iconUrl: 'assets/marker-icon.png',
      shadowUrl: 'assets/marker-shadow.png',
    });
  }

  ngAfterViewInit(): void {
    this.initMap();
    this.initLayers();
    this.subscribeToStore();
    this.handleDeepLink();
  }

  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
    this.routeLayer?.remove();
    this.map?.remove();
  }

  private initMap(): void {
    const { center, defaultZoom, minZoom, maxZoom, tileUrl, tileAttribution } = environment.map;

    this.map = L.map(this.mapContainer.nativeElement, {
      center: [center.lat, center.lng],
      zoom: defaultZoom,
      minZoom,
      maxZoom,
      zoomControl: true,
      attributionControl: true,
    });

    L.tileLayer(tileUrl, { attribution: tileAttribution }).addTo(this.map);

    this.map.on('zoomend', () => {
      this.store.dispatch(setMapZoom({ zoom: this.map.getZoom() }));
    });
  }

  private initLayers(): void {
    this.stationLayer = new StationLayer(
      this.accessibility,
      (station) => this.onStationClicked(station)
    );

    this.lineLayer = new LineLayer(this.accessibility);
    this.routeLayer = new RouteLayer(this.accessibility);

    this.lineLayer.addTo(this.map);
    this.stationLayer.addTo(this.map);
    this.routeLayer.addTo(this.map);

    // Add legend control
    new LegendControl({ position: 'bottomright' }).addTo(this.map);
  }

  private subscribeToStore(): void {
    // Render stations when loaded
    this.store
      .select(selectAllStations)
      .pipe(takeUntil(this.destroy$))
      .subscribe((stations) => {
        stations.forEach((s) => this.stationMap.set(s.id, s));
        this.stationLayer.setStations(stations);
        this.lineLayer.setStations(stations);
      });

    // Render route when computed
    this.store
      .select(selectCurrentRoute)
      .pipe(takeUntil(this.destroy$))
      .subscribe((route) => {
        if (route) {
          this.routeLayer.displayRoute(route, this.stationMap);
          const bounds = this.routeLayer.getBounds();
          if (bounds) {
            this.map.fitBounds(bounds, { padding: [50, 50] });
          }
        } else {
          this.routeLayer.clear();
        }
      });

    // Track departure selection to toggle click target
    this.store
      .select(selectDeparture)
      .pipe(takeUntil(this.destroy$))
      .subscribe((departure) => {
        this.isSelectingDeparture = departure === null;
      });
  }

  /**
   * Handle deep links in the format /route/:fromId/:toId.
   * Fetches station details and auto-computes the route.
   */
  private handleDeepLink(): void {
    this.route.paramMap
      .pipe(
        filter((params) => params.has('fromId') && params.has('toId')),
        take(1),
        switchMap((params) => {
          const fromId = Number(params.get('fromId'));
          const toId = Number(params.get('toId'));
          return combineLatest([
            this.stationService.getById(fromId),
            this.stationService.getById(toId),
          ]);
        }),
        takeUntil(this.destroy$)
      )
      .subscribe(([departure, arrival]) => {
        this.store.dispatch(setDeparture({ station: departure }));
        this.store.dispatch(setArrival({ station: arrival }));
        this.store.dispatch(computeRoute());
      });
  }

  private onStationClicked(station: Station): void {
    if (this.isSelectingDeparture) {
      this.store.dispatch(setDeparture({ station }));
    } else {
      this.store.dispatch(setArrival({ station }));
    }
  }
}
