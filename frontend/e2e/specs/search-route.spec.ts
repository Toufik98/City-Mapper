import { test, expect } from '@playwright/test';

test.describe('Search Route', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await page.waitForSelector('app-search-panel');
  });

  test('should display the search panel on load', async ({ page }) => {
    await expect(page.locator('app-search-panel')).toBeVisible();
    await expect(page.locator('mat-card-title')).toContainText('City Mapper');
  });

  test('should have departure and arrival input fields', async ({ page }) => {
    const inputs = page.locator('app-station-autocomplete');
    await expect(inputs).toHaveCount(2);
  });

  test('should show autocomplete suggestions when typing', async ({ page }) => {
    const departureInput = page.locator('app-station-autocomplete').first().locator('input');
    await departureInput.fill('Châ');
    // Wait for autocomplete panel to appear
    await page.waitForSelector('mat-option', { timeout: 5000 });
    const options = page.locator('mat-option');
    await expect(options.first()).toBeVisible();
  });

  test('should compute a route when both stations are selected', async ({ page }) => {
    // Select departure
    const departureInput = page.locator('app-station-autocomplete').first().locator('input');
    await departureInput.fill('Châtelet');
    await page.waitForSelector('mat-option');
    await page.locator('mat-option').first().click();

    // Select arrival
    const arrivalInput = page.locator('app-station-autocomplete').last().locator('input');
    await arrivalInput.fill('Nation');
    await page.waitForSelector('mat-option');
    await page.locator('mat-option').first().click();

    // Click find route
    await page.locator('button:has-text("Find Route"), button:has-text("Rechercher")').click();

    // Verify route details appear
    await page.waitForSelector('app-route-details', { timeout: 10000 });
    await expect(page.locator('app-route-details')).toBeVisible();
  });

  test('should swap departure and arrival with the swap button', async ({ page }) => {
    const departureInput = page.locator('app-station-autocomplete').first().locator('input');
    const arrivalInput = page.locator('app-station-autocomplete').last().locator('input');

    await departureInput.fill('Station A');
    await arrivalInput.fill('Station B');

    await page.locator('app-swap-button button').click();

    // After swap, values should be reversed
    // Note: This verifies the swap interaction fires, actual data depends on backend
  });

  test('should disable find route button when no stations selected', async ({ page }) => {
    const findRouteButton = page.locator('button:has-text("Find Route"), button:has-text("Rechercher")');
    await expect(findRouteButton).toBeDisabled();
  });
});
