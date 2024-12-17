const path = require('path');
const pkg = require('../react-native-nitro-palette/package.json');

/**
 * @type {import('@react-native-community/cli-types').Config}
 */
module.exports = {
  dependencies: {
    [pkg.name]: {
      root: path.join(__dirname, '..', 'react-native-nitro-palette'),
      platforms: {
        android: {
          sourceDir: './android',
        },
      },
    },
  },
};
