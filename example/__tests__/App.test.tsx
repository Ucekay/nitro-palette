/**
 * @format
 */

import "react-native";
import App from "../App";

// Note: import explicitly to use the types shipped with jest.
import { it, jest } from "@jest/globals";

// Note: test renderer must be required after react-native.
import renderer from "react-test-renderer";

// Nitro / Skia / color-thief need their native runtime, which Jest doesn't have.
jest.mock("react-native-nitro-palette", () => ({
	getPaletteAsync: async () => [],
}));
jest.mock("react-native-color-thief", () => ({
	__esModule: true,
	default: { getColor: async () => ({}), getPalette: async () => [] },
}));

it("renders correctly", () => {
	renderer.create(<App />);
});
