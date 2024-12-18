import {
	AlphaType,
	ColorType,
	type DataSourceParam,
	type SkImage,
	Skia,
	loadData,
} from "@shopify/react-native-skia";
import { NitroPalette } from "./specs";

const imgFactory = Skia.Image.MakeImageFromEncoded.bind(Skia.Image);

export const getPaletteAsync = async (
	source: DataSourceParam | string,
	colorCount = 5,
	quality = 10,
	ignoreWhite = true,
): Promise<string[]> => {
	try {
		let image: SkImage | null = null;
		if (typeof source === "string" && source.startsWith("file://")) {
			const imageData = await Skia.Data.fromURI(source);
			image = Skia.Image.MakeImageFromEncoded(imageData);
		} else {
			image = await loadData(source, imgFactory);
		}
		if (!image) {
			throw new Error("Failed to create image");
		}
		const pixels = image.readPixels(0, 0, {
			width: image.width(),
			height: image.height(),
			colorType: ColorType.RGBA_8888,
			alphaType: AlphaType.Unpremul,
		});
		if (!pixels) {
			throw new Error("Failed to read pixels");
		}
		return NitroPalette.extractColorsAsync(
			pixels.buffer as ArrayBuffer,
			colorCount,
			quality,
			ignoreWhite,
		);
	} catch (error) {
		throw new Error(error instanceof Error ? error.message : String(error));
	}
};
