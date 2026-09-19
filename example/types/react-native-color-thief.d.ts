declare module "react-native-color-thief" {
	export type RGBColor = { r: number; g: number; b: number };

	const RNColorThief: {
		getColor(
			imageUri: string,
			quality: number,
			includeWhite: boolean,
		): Promise<RGBColor>;
		getPalette(
			imageUri: string,
			colorCount: number,
			quality: number,
			includeWhite: boolean,
		): Promise<RGBColor[]>;
	};

	export default RNColorThief;
}
