#include <NitroModules/ArrayBuffer.hpp>
#include "NitroPalette.hpp"
#include "MMCQ.hpp"

#include <iostream>
#include <chrono>

namespace margelo {
namespace nitro {
namespace nitropalette {

std::vector<std::string> NitroPalette::extractColors(
    const std::shared_ptr<ArrayBuffer>& source, double colorCount,
    double quality, bool ignoreWhite) {
  currentImageSize_ = source->size();

  if (!source || currentImageSize_ < 4 || currentImageSize_ % 4 != 0) {
    throw std::runtime_error("Invalid source buffer size or format");
  }

  colorCount = std::clamp(colorCount, 1.0, 20.0);
  quality = std::clamp(quality, 1.0, 10.0);
  const int colorCountInt = static_cast<int>(colorCount);

  auto pixels = source->data();
  std::vector<uint8_t> pixelsVector(pixels, pixels + currentImageSize_);

  auto colorMap = MMCQ::quantize(pixelsVector, colorCountInt,
                                 static_cast<int>(quality), ignoreWhite);

  auto palette = colorMap->makePalette();

  if (palette.empty()) {
    return {};
  }

  std::vector<std::string> result;
  result.reserve(palette.size());
  for (const auto& color : palette) {
    result.push_back(color.toString());
  }

  return result;
}

std::shared_ptr<Promise<std::vector<std::string>>>
NitroPalette::extractColorsAsync(const std::shared_ptr<ArrayBuffer>& source,
                                 double colorCount, double quality,
                                 bool ignoreWhite) {
  if (!source) {
    throw std::runtime_error("Source buffer is null");
  }

  uint8_t* sourceData = source->data();
  size_t sourceSize = source->size();

  if (sourceData == nullptr) {
    throw std::runtime_error("Invalid source buffer data");
  }

  std::vector<uint8_t> sourceCopy(sourceData, sourceData + sourceSize);

  return Promise<std::vector<std::string>>::async(
      [this, sourceCopy = std::move(sourceCopy), colorCount, quality,
       ignoreWhite]() {
        currentImageSize_ = sourceCopy.size();

        if (sourceCopy.empty() || currentImageSize_ < 4 ||
            currentImageSize_ % 4 != 0) {
          throw std::runtime_error("Invalid source buffer size or format");
        }

        const double clampedColorCount = std::clamp(colorCount, 1.0, 20.0);
        const double clampedQuality = std::clamp(quality, 1.0, 10.0);
        const int colorCountInt = static_cast<int>(clampedColorCount);

        auto colorMap =
            MMCQ::quantize(sourceCopy, colorCountInt,
                           static_cast<int>(clampedQuality), ignoreWhite);

        auto palette = colorMap->makePalette();

        if (palette.empty()) {
          return std::vector<std::string>();
        }

        std::vector<std::string> result;
        result.reserve(palette.size());
        for (const auto& color : palette) {
          result.push_back(color.toString());
        }

        return result;
      });
}
}  // namespace nitropalette
}  // namespace nitro
}  // namespace margelo