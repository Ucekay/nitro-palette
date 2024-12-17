# react-native-nitro-palette

A fast and efficient color palette extraction library for React Native, powered by C++ and the MMCQ (Modified Median Cut Quantization) algorithm.

## Features

- 🚀 High-performance color extraction using native C++
- 🎨 Extracts dominant colors from images
- 🔧 Configurable color count and quality settings

## Installation

```sh
npm install react-native-nitro-palette react-native-nitro-modules react-native-skia
# or
yarn add react-native-nitro-palette react-native-nitro-modules react-native-skia
```

### Additional Setup

#### For Expo projects
```sh
yarn expo prebuild --clean
```

#### For React Native projects
```sh
cd ios && pod install
```

## Usage

```typescript
import { getPaletteAsync } from 'react-native-nitro-palette';

// Extract colors from a local image
const colors = await getPaletteAsync(
  'file:///path/to/image.jpg',  // local file path, remote URL, or base64 data
  5,                            // number of colors to extract (default: 5)
  10,                           // quality (1 = best, 10 = fastest, default: 10)
  true                         // ignore white colors (default: true)
);

console.log(colors); // ['rgb(255,0,0)', 'rgb(0,255,0)', ...]
```

## API Reference

### getPaletteAsync(source, colorCount?, quality?, ignoreWhite?)

Extract colors from an image source.

#### Parameters

- `source` (string) - The image source (local file path, remote URL, or base64 data)
- `colorCount` (number) - Optional. Number of colors to extract (default: 5)
- `quality` (number) - Optional. Quality of color extraction (1 = best, 10 = fastest, default: 10)
- `ignoreWhite` (boolean) - Optional. Whether to ignore white colors (default: true)

#### Returns

- `Promise<string[]>` - Array of RGB colors in CSS format (e.g., 'rgb(255,0,0)')

## License

MIT

## Credits

This library is a C++ port of [ColorThiefSwift](https://github.com/yamoridon/ColorThiefSwift), which is a Swift implementation of ColorThief. The MMCQ (Modified Median Cut Quantization) algorithm implementation is based on the original work.

### Related Projects
- [ColorThiefSwift](https://github.com/yamoridon/ColorThiefSwift)
- [Color Thief](https://lokeshdhakar.com/projects/color-thief/)