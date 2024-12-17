#include <vector>
#include <string>
#include <NitroModules/ArrayBuffer.hpp>
#include "HybridNitroPaletteSpec.hpp"

namespace margelo {
namespace nitro {
namespace nitropalette {
class NitroPalette : public HybridNitroPaletteSpec {
 public:
  NitroPalette() : HybridObject(TAG), HybridNitroPaletteSpec() {}

  std::vector<std::string> extractColors(
      const std::shared_ptr<ArrayBuffer>& source, double colorCount,
      double quality, bool ignoreWhite) override;

  size_t getExternalMemorySize() noexcept override {
    return sizeof(NitroPalette) + currentImageSize_;
  }

 private:
  size_t currentImageSize_ = 0;
};

}  // namespace nitropalette
}  // namespace nitro
}  // namespace margelo