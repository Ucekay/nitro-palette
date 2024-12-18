import type { HybridObject } from "react-native-nitro-modules";

export interface NitroPalette
	extends HybridObject<{ ios: "c++"; android: "c++" }> {
	extractColors(
		source: ArrayBuffer,
		colorCount: number,
		quality: number,
		ignoreWhite: boolean,
	): string[];
	extractColorsAsync(
		source: ArrayBuffer,
		colorCount: number,
		quality: number,
		ignoreWhite: boolean,
	): Promise<string[]>;
}
