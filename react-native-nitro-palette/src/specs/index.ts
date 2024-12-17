import { NitroModules } from "react-native-nitro-modules";
import type { NitroPalette as NitroPaletteSpec } from "./NitroPalette.nitro";

export const NitroPalette =
	NitroModules.createHybridObject<NitroPaletteSpec>("NitroPalette");
