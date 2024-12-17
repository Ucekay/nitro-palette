///
/// HybridNitroPaletteSpec.cpp
/// This file was generated by nitrogen. DO NOT MODIFY THIS FILE.
/// https://github.com/mrousavy/nitro
/// Copyright © 2024 Marc Rousavy @ Margelo
///

#include "HybridNitroPaletteSpec.hpp"

namespace margelo::nitro::nitropalette {

  void HybridNitroPaletteSpec::loadHybridMethods() {
    // load base methods/properties
    HybridObject::loadHybridMethods();
    // load custom methods/properties
    registerHybrids(this, [](Prototype& prototype) {
      prototype.registerHybridMethod("extractColors", &HybridNitroPaletteSpec::extractColors);
    });
  }

} // namespace margelo::nitro::nitropalette
