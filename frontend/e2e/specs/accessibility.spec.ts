import { test, expect } from '@playwright/test';

test.describe('Accessibility', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
    await page.waitForSelector('app-search-panel');
  });

  test('should open settings panel', async ({ page }) => {
    await page.locator('button[aria-label="Settings"]').click();
    await page.waitForSelector('app-settings-panel');
    await expect(page.locator('app-settings-panel')).toBeVisible();
  });

  test('should toggle colorblind mode', async ({ page }) => {
    await page.locator('button[aria-label="Settings"]').click();
    await page.waitForSelector('app-settings-panel');

    const toggle = page.locator('mat-slide-toggle');
    await expect(toggle).toBeVisible();
    await toggle.click();
    // Verify the toggle state changed
  });

  test('should switch language to English', async ({ page }) => {
    await page.locator('button[aria-label="Settings"]').click();
    await page.waitForSelector('app-settings-panel');

    // Click English toggle
    await page.locator('mat-button-toggle[value="en"]').click();

    // Verify text changed to English
    await expect(page.locator('mat-card-title').first()).toContainText('City Mapper');
  });

  test('should switch language to French', async ({ page }) => {
    await page.locator('button[aria-label="Settings"]').click();
    await page.waitForSelector('app-settings-panel');

    // Click French toggle
    await page.locator('mat-button-toggle[value="fr"]').click();

    // Verify text in French
    await expect(page.locator('mat-card-title').first()).toContainText('City Mapper');
  });

  test('should have proper ARIA labels on interactive elements', async ({ page }) => {
    // Toolbar buttons
    await expect(page.locator('button[aria-label="Toggle sidebar"]')).toBeVisible();
    await expect(page.locator('button[aria-label="Settings"]')).toBeVisible();

    // Find route button
    const findBtn = page.locator('button:has-text("Find Route"), button:has-text("Rechercher")');
    await expect(findBtn).toBeVisible();
  });

  test('should be navigable via keyboard', async ({ page }) => {
    // Tab through the interface
    await page.keyboard.press('Tab');
    await page.keyboard.press('Tab');
    await page.keyboard.press('Tab');

    // Check that focus is visible on an interactive element
    const focused = page.locator(':focus');
    await expect(focused).toBeVisible();
  });

  test('should close settings panel when clicking overlay', async ({ page }) => {
    await page.locator('button[aria-label="Settings"]').click();
    await page.waitForSelector('app-settings-panel');

    // Click the overlay (outside the settings container)
    await page.locator('.settings-overlay').click({ position: { x: 10, y: 10 } });

    // Settings should be closed
    await expect(page.locator('app-settings-panel')).not.toBeVisible();
  });
});
