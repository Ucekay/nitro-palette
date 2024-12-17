#ifndef MMCQ_HPP
#define MMCQ_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include <iostream>

class MMCQ {
 public:
  struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Color() : r(0), g(0), b(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    std::string toString() const;
  };

  enum ColorChannel { R, G, B };

  class VBox {
   public:
    struct Range {
      int begin;
      int end;
    };

    VBox(uint8_t rMin, uint8_t rMax, uint8_t gMin, uint8_t gMax, uint8_t bMin,
         uint8_t bMax, std::vector<int>& histogram);
    VBox(const VBox& vbox);
    VBox& operator=(const VBox& other);
    VBox& operator=(VBox&& other) noexcept = default;

    int getVolume(bool forceRecalculation = false) const;
    int getCount(bool forceRecalculation = false) const;
    Color getAverage(bool forceRecalculation = false) const;
    ColorChannel widestColorChannel() const;

    uint8_t rMin, rMax;
    uint8_t gMin, gMax;
    uint8_t bMin, bMax;

   private:
    std::shared_ptr<std::vector<int>> histogram;
    mutable std::optional<Color> average;
    mutable std::optional<int> volume;
    mutable std::optional<int> count;
  };

  class ColorMap {
   public:
    ColorMap() = default;
    ColorMap(const ColorMap& other);
    ColorMap& operator=(const ColorMap& other);
    ColorMap(ColorMap&& other) noexcept = default;
    ColorMap& operator=(ColorMap&& other) noexcept = default;
    std::vector<Color> makePalette() const;
    Color makeNearestColor(const Color& color) const;
    void push(const VBox& vbox);

   private:
    std::vector<VBox> vboxes;
  };

  static std::unique_ptr<ColorMap> quantize(const std::vector<uint8_t>& pixels,
                                            int maxColors, int quality,
                                            bool ignoreWhite);

 private:
  static constexpr int SIGNAL_BITS = 5;
  static constexpr int RIGHT_SHIFT = 8 - SIGNAL_BITS;
  static constexpr int MULTIPLIER = 1 << RIGHT_SHIFT;
  static constexpr int HISTOGRAM_SIZE = 1 << (3 * SIGNAL_BITS);
  static constexpr int VBOX_LENGTH = 1 << SIGNAL_BITS;
  static constexpr double FRACTION_BY_POPULATION = 0.75;
  static constexpr int MAX_ITERATIONS = 1000;

  static int makeColorIndexOf(int red, int green, int blue);

  static std::pair<std::vector<int, std::allocator<int>>, VBox>
  makeHistogramAndBox(
      const std::vector<uint8_t, std::allocator<uint8_t>>& pixels, int quality,
      bool ignoreWhite);

  static std::vector<VBox, std::allocator<VBox>> applyMedianCut(
      const std::vector<int, std::allocator<int>>& histogram, const VBox& vbox);

  static std::vector<VBox> cut(ColorChannel axis, const VBox& vbox, int vboxMin,
                               int vboxMax, const std::vector<int>& partialSun,
                               const std::vector<int>& lookAheadSum, int total);

  static void iterate(std::vector<VBox>& queue,
                      bool (*comparator)(const VBox&, const VBox&), int target,
                      const std::vector<int>& histogram);

  static bool compareByCount(const VBox& a, const VBox& b);
  static bool compareByProduct(const VBox& a, const VBox& b);
};

#endif