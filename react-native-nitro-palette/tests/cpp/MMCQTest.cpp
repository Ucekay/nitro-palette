// Behavior tests for the MMCQ color quantizer.
// Run with `bun run test:cpp` from the package directory.

#include "MMCQ.hpp"

#include <cstdio>
#include <string>
#include <vector>

namespace {

int failures = 0;

#define CHECK(cond)                                             \
  do {                                                          \
    if (!(cond)) {                                              \
      std::printf("  FAILED: %s (line %d)\n", #cond, __LINE__); \
      failures++;                                               \
    }                                                           \
  } while (0)

struct Fill {
  uint8_t r, g, b, a;
  size_t count;
};

std::vector<uint8_t> makePixels(const std::vector<Fill>& fills) {
  std::vector<uint8_t> pixels;
  for (const auto& fill : fills) {
    for (size_t i = 0; i < fill.count; i++) {
      pixels.insert(pixels.end(), {fill.r, fill.g, fill.b, fill.a});
    }
  }
  return pixels;
}

std::vector<std::string> palette(const std::vector<uint8_t>& pixels,
                                 int maxColors, int quality,
                                 bool ignoreWhite) {
  std::vector<std::string> result;
  auto colorMap = MMCQ::quantize(pixels, maxColors, quality, ignoreWhite);
  if (colorMap) {
    for (const auto& color : colorMap->makePalette()) {
      result.push_back(color.toString());
    }
  }
  return result;
}

bool contains(const std::vector<std::string>& colors,
              const std::string& color) {
  for (const auto& c : colors) {
    if (c == color) return true;
  }
  return false;
}

void test(const char* name, void (*fn)()) {
  std::printf("%s\n", name);
  fn();
}

}  // namespace

int main() {
  test("solid image returns its color first", [] {
    auto colors = palette(makePixels({{200, 0, 0, 255, 1000}}), 5, 1, true);
    CHECK(!colors.empty());
    CHECK(!colors.empty() && colors[0] == "rgb(204,4,4)");
  });

  test("dominant color is kept", [] {
    auto colors = palette(
        makePixels({{200, 0, 0, 255, 900}, {0, 0, 200, 255, 100}}), 5, 1,
        true);
    CHECK(contains(colors, "rgb(204,4,4)"));
    CHECK(contains(colors, "rgb(4,4,204)"));
  });

  test("equally distributed colors are all kept", [] {
    auto colors = palette(makePixels({{200, 0, 0, 255, 300},
                                      {0, 200, 0, 255, 300},
                                      {0, 0, 200, 255, 300}}),
                          5, 1, true);
    CHECK(contains(colors, "rgb(204,4,4)"));
    CHECK(contains(colors, "rgb(4,204,4)"));
    CHECK(contains(colors, "rgb(4,4,204)"));
  });

  test("large images do not overflow the average", [] {
    auto colors =
        palette(makePixels({{240, 240, 240, 255, 9'000'000}}), 5, 1, true);
    CHECK(!colors.empty() && colors[0] == "rgb(244,244,244)");
  });

  test("mostly transparent pixels are ignored", [] {
    auto colors = palette(
        makePixels({{200, 0, 0, 255, 100}, {0, 0, 200, 124, 1000}}), 5, 1,
        true);
    CHECK(!colors.empty() && colors[0] == "rgb(204,4,4)");
    CHECK(!contains(colors, "rgb(4,4,204)"));
  });

  test("white is ignored only when requested", [] {
    auto pixels =
        makePixels({{255, 255, 255, 255, 1000}, {0, 200, 0, 255, 100}});
    auto ignored = palette(pixels, 5, 1, true);
    CHECK(!ignored.empty() && ignored[0] == "rgb(4,204,4)");
    CHECK(!contains(ignored, "rgb(252,252,252)"));
    CHECK(contains(palette(pixels, 5, 1, false), "rgb(252,252,252)"));
  });

  if (failures > 0) {
    std::printf("%d check(s) failed\n", failures);
    return 1;
  }
  std::printf("all checks passed\n");
  return 0;
}
