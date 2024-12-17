declare module "react-native-nitro-palette" {
	/**
	 * Extracts a color palette from an image
	 * @param source - The image source (local file path, remote URL, or base64 data)
	 * @param colorCount - The number of colors to extract
	 * @param quality - The quality of the palette extraction (1 = best, 10 = fastest)
	 * @param ignoreWhite - Whether to ignore white colors
	 * @returns An array of colors in hex format
	 */
	export function getPaletteAsync(
		source: string,
		colorCount?: number,
		quality?: number,
		ignoreWhite?: boolean,
	): string[];
}
