/**
 * Colorblind-safe alternative palette for Paris Metro lines.
 * Uses high-contrast colors distinguishable in all common forms of color blindness
 * (protanopia, deuteranopia, tritanopia).
 */
export const METRO_LINE_COLORS_ACCESSIBLE: Record<string, string> = {
  '1':    '#E69F00',  // Orange
  '2':    '#0072B2',  // Blue
  '3':    '#009E73',  // Bluish Green
  '3bis': '#56B4E9', // Sky Blue
  '4':    '#CC79A7',  // Reddish Purple
  '5':    '#D55E00',  // Vermillion
  '6':    '#F0E442',  // Yellow
  '7':    '#CC79A7',  // Reddish Purple (lighter)
  '7bis': '#009E73', // Bluish Green
  '8':    '#56B4E9',  // Sky Blue
  '9':    '#E69F00',  // Orange
  '10':   '#0072B2',  // Blue
  '11':   '#D55E00',  // Vermillion
  '12':   '#009E73',  // Bluish Green
  '13':   '#56B4E9',  // Sky Blue
  '14':   '#CC79A7',  // Reddish Purple
};

/**
 * Dash patterns for each line (used alongside color in accessible mode).
 * Format: [dashLength, gapLength] for Leaflet polylines.
 */
export const METRO_LINE_DASH_PATTERNS: Record<string, string> = {
  '1':    '',            // Solid
  '2':    '',            // Solid
  '3':    '10 5',        // Dashed
  '3bis': '5 5',        // Short dash
  '4':    '',            // Solid
  '5':    '15 5 5 5',    // Dash-dot
  '6':    '2 4',         // Dotted
  '7':    '10 5',        // Dashed
  '7bis': '5 5',        // Short dash
  '8':    '15 5 5 5',    // Dash-dot
  '9':    '2 4',         // Dotted
  '10':   '10 5',        // Dashed
  '11':   '5 5',         // Short dash
  '12':   '15 5 5 5',    // Dash-dot
  '13':   '2 4',         // Dotted
  '14':   '',            // Solid
};
