# react-native-nitro-palette

A color palette package for React Native/Expo that extracts dominant colors from images.

## Features
- High-performance color extraction powered by C++ implementation using Nitro Modules
- Support for both local and remote images
- Customizable number of colors to extract
- Adjustable quality settings for performance optimization
- Option to filter out white/transparent colors
- Full TypeScript support

## Demo

### Remote Image Color Extraction
https://github.com/user-attachments/assets/78b2f760-7345-423a-9eaa-1d3558baafe3

### Device Library Image Color Extraction

## Usage

For detailed implementation examples and usage instructions, please see the README.md in the react-native-nitro-palette directory.

### Basic Usage Example

```javascript
import { getPaletteAsync } from 'react-native-nitro-palette';

const colors = await getPaletteAsync(
  'https://example.jpg',
  5,                            // number of colors to extract (default: 5)
  10,                           // quality (1 = best, 10 = fastest, default: 10)
  true                          // ignore white colors (default: true)
);

console.log(colors); // ['rgb(255,0,0)', 'rgb(0,255,0)', ...]
```

## Running Example App

The example application demonstrates the full capabilities of react-native-nitro-palette.

> **Note**: Currently, the example app is only configured to run on iOS. However, the react-native-nitro-palette package itself is fully compatible with both iOS and Android platforms.

### To run the example:

1. Clone this repository
2. Navigate to the example directory: `cd example`
3. Install dependencies: `npm install` or `yarn install`
4. For iOS:
   - Install pods: `cd ios && pod install`
   - Run: `npm run ios` or `yarn ios`

