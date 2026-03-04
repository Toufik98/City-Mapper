/**
 * RATP official Paris Metro line colors.
 * Source: https://www.ratp.fr/
 */
export const METRO_LINE_COLORS: Record<string, string> = {
  '1':    '#FFCD00',  // Yellow
  '2':    '#003CA6',  // Blue
  '3':    '#837902',  // Olive
  '3bis': '#6EC4E8', // Light Blue
  '4':    '#CF009E',  // Magenta
  '5':    '#FF7E2E',  // Orange
  '6':    '#6ECA97',  // Light Green
  '7':    '#FA9ABA',  // Pink
  '7bis': '#6ECA97', // Light Green
  '8':    '#E19BDF',  // Lilac
  '9':    '#B6BD00',  // Yellow-Green
  '10':   '#C9910D',  // Brown-Gold
  '11':   '#704B1C',  // Brown
  '12':   '#007852',  // Dark Green
  '13':   '#6EC4E8',  // Light Blue
  '14':   '#62259D',  // Purple
};

/**
 * Text color (white or black) for optimal contrast on each line's background.
 */
export const METRO_LINE_TEXT_COLORS: Record<string, string> = {
  '1':    '#000000',
  '2':    '#FFFFFF',
  '3':    '#FFFFFF',
  '3bis': '#000000',
  '4':    '#FFFFFF',
  '5':    '#000000',
  '6':    '#000000',
  '7':    '#000000',
  '7bis': '#000000',
  '8':    '#000000',
  '9':    '#000000',
  '10':   '#FFFFFF',
  '11':   '#FFFFFF',
  '12':   '#FFFFFF',
  '13':   '#000000',
  '14':   '#FFFFFF',
};

/**
 * Default fallback color for unknown lines.
 */
export const DEFAULT_LINE_COLOR = '#888888';
export const DEFAULT_TEXT_COLOR = '#FFFFFF';
