#include "MMCQ.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <utility>

std::string MMCQ::Color::toString() const {
  std::ostringstream oss;
  oss << "rgb(" << static_cast<int>(r) << "," << static_cast<int>(g) << ","
      << static_cast<int>(b) << ")";
  return oss.str();
}

std::vector<MMCQ::Color> MMCQ::ColorMap::makePalette() const {
  std::vector<Color> palette;
  palette.reserve(vboxes.size());

  for (const auto& vbox : vboxes) {
    palette.push_back(vbox.getAverage());
  }

  return palette;
}

MMCQ::Color MMCQ::ColorMap::makeNearestColor(const MMCQ::Color& color) const {
  int minDistance = std::numeric_limits<int>::max();
  MMCQ::Color nearestColor(0, 0, 0);

  for (const auto& vbox : vboxes) {
    MMCQ::Color vboxColor = vbox.getAverage();
    int dr =
        std::abs(static_cast<int>(color.r) - static_cast<int>(vboxColor.r));
    int dg =
        std::abs(static_cast<int>(color.g) - static_cast<int>(vboxColor.g));
    int db =
        std::abs(static_cast<int>(color.b) - static_cast<int>(vboxColor.b));
    int distance = dr + dg + db;
    if (distance < minDistance) {
      minDistance = distance;
      nearestColor = vboxColor;
    }
  }
  return nearestColor;
}

void MMCQ::ColorMap::push(const MMCQ::VBox& vbox) { vboxes.push_back(vbox); }

MMCQ::ColorMap::ColorMap(const ColorMap& other) : vboxes(other.vboxes) {}

MMCQ::ColorMap& MMCQ::ColorMap::operator=(const ColorMap& other) {
  if (this != &other) {
    vboxes = other.vboxes;
  }
  return *this;
}

MMCQ::VBox::VBox(uint8_t rMin, uint8_t rMax, uint8_t gMin, uint8_t gMax,
                 uint8_t bMin, uint8_t bMax, std::vector<int>& histogram)
    : rMin(rMin),
      rMax(rMax),
      gMin(gMin),
      gMax(gMax),
      bMin(bMin),
      bMax(bMax),
      histogram(std::make_shared<std::vector<int>>(histogram)) {}

MMCQ::VBox::VBox(const VBox& vbox)
    : rMin(vbox.rMin),
      rMax(vbox.rMax),
      gMin(vbox.gMin),
      gMax(vbox.gMax),
      bMin(vbox.bMin),
      bMax(vbox.bMax),
      histogram(vbox.histogram) {}

MMCQ::VBox& MMCQ::VBox::operator=(const VBox& other) {
  if (this != &other) {
    rMin = other.rMin;
    rMax = other.rMax;
    gMin = other.gMin;
    gMax = other.gMax;
    bMin = other.bMin;
    bMax = other.bMax;
    histogram = other.histogram;
    average = other.average;
    volume = other.volume;
    count = other.count;
  }
  return *this;
}

int MMCQ::VBox::getVolume(bool forceRecalculation) const {
  if (!forceRecalculation && volume.has_value()) {
    return volume.value();
  } else {
    int newVolume = (static_cast<int>(rMax) - rMin + 1) *
                    (static_cast<int>(gMax) - gMin + 1) *
                    (static_cast<int>(bMax) - bMin + 1);
    return newVolume;
  }
}

int MMCQ::VBox::getCount(bool forceRecalculation) const {
  if (!forceRecalculation && count.has_value()) {
    return count.value();
  } else {
    int totalCount = 0;
    for (int i = rMin; i <= rMax; i++) {
      for (int j = gMin; j <= gMax; j++) {
        for (int k = bMin; k <= bMax; k++) {
          int index = MMCQ::makeColorIndexOf(i, j, k);
          totalCount += histogram->at(index);
        }
      }
    }
    count = totalCount;
    return totalCount;
  }
}

MMCQ::Color MMCQ::VBox::getAverage(bool forceRecalculation) const {
  if (!forceRecalculation && average.has_value()) {
    return average.value();
  } else {
    int histogramValueSum = 0;
    int rSum = 0, gSum = 0, bSum = 0;

    for (int i = rMin; i <= rMax; i++) {
      for (int j = gMin; j <= gMax; j++) {
        for (int k = bMin; k <= bMax; k++) {
          int index = MMCQ::makeColorIndexOf(i, j, k);
          int histogramValue = histogram->at(index);
          if (histogramValue == 0) {
            continue;
          }
          histogramValueSum += histogramValue;
          rSum += static_cast<int>(histogramValue * (i + 0.5) * MULTIPLIER);
          gSum += static_cast<int>(histogramValue * (j + 0.5) * MULTIPLIER);
          bSum += static_cast<int>(histogramValue * (k + 0.5) * MULTIPLIER);
        }
      }
    }

    average.emplace(histogramValueSum > 0
                        ? Color(static_cast<uint8_t>(rSum / histogramValueSum),
                                static_cast<uint8_t>(gSum / histogramValueSum),
                                static_cast<uint8_t>(bSum / histogramValueSum))
                        : Color(static_cast<uint8_t>(
                                    std::min(MULTIPLIER *
                                                 (static_cast<int>(rMin) +
                                                  static_cast<int>(rMax) + 1) /
                                                 2,
                                             255)),
                                static_cast<uint8_t>(
                                    std::min(MULTIPLIER *
                                                 (static_cast<int>(gMin) +
                                                  static_cast<int>(gMax) + 1) /
                                                 2,
                                             255)),
                                static_cast<uint8_t>(
                                    std::min(MULTIPLIER *
                                                 (static_cast<int>(bMin) +
                                                  static_cast<int>(bMax) + 1) /
                                                 2,
                                             255))));
    return average.value();
  }
}

MMCQ::ColorChannel MMCQ::VBox::widestColorChannel() const {
  int rWidth = rMax - rMin;
  int gWidth = gMax - gMin;
  int bWidth = bMax - bMin;

  if (rWidth >= gWidth && rWidth >= bWidth) {
    return ColorChannel::R;
  } else if (gWidth >= rWidth && gWidth >= bWidth) {
    return ColorChannel::G;
  } else {
    return ColorChannel::B;
  }
}

std::unique_ptr<MMCQ::ColorMap> MMCQ::quantize(
    const std::vector<uint8_t>& pixels, int maxColors, int quality,
    bool ignoreWhite) {
  if (pixels.empty() || maxColors < 1 || maxColors > 255) {
    return nullptr;
  }

  std::pair<std::vector<int, std::allocator<int>>, VBox> histogramAndBox =
      makeHistogramAndBox(pixels, quality, ignoreWhite);
  std::vector<VBox> pqueue;
  pqueue.push_back(histogramAndBox.second);
  int target = static_cast<int>(FRACTION_BY_POPULATION * maxColors);
  iterate(pqueue, compareByCount, target, histogramAndBox.first);
  std::sort(pqueue.begin(), pqueue.end(), compareByProduct);
  iterate(pqueue, compareByProduct, maxColors, histogramAndBox.first);
  std::reverse(pqueue.begin(), pqueue.end());

  MMCQ::ColorMap colorMap;
  for (const auto& vbox : pqueue) {
    colorMap.push(vbox);
  }
  return std::make_unique<ColorMap>(colorMap);
}

int MMCQ::makeColorIndexOf(int red, int green, int blue) {
  return (red << (2 * SIGNAL_BITS)) + (green << SIGNAL_BITS) + blue;
}

std::pair<std::vector<int, std::allocator<int>>, MMCQ::VBox>
MMCQ::makeHistogramAndBox(
    const std::vector<uint8_t, std::allocator<uint8_t>>& pixels, int quality,
    bool ignoreWhite) {
  std::vector<int> histogram(HISTOGRAM_SIZE, 0);
  uint8_t rMin = std::numeric_limits<uint8_t>::max();
  uint8_t gMin = std::numeric_limits<uint8_t>::max();
  uint8_t bMin = std::numeric_limits<uint8_t>::max();
  uint8_t rMax = std::numeric_limits<uint8_t>::min();
  uint8_t gMax = std::numeric_limits<uint8_t>::min();
  uint8_t bMax = std::numeric_limits<uint8_t>::min();

  size_t pixelCount = pixels.size() / 4;
  for (size_t i = 0; i < pixelCount; i += quality) {
    uint8_t r = pixels[i * 4];
    uint8_t g = pixels[i * 4 + 1];
    uint8_t b = pixels[i * 4 + 2];
    uint8_t a = pixels[i * 4 + 3];

    if (a <= 125 || (ignoreWhite && r > 250 && g > 250 && b > 250)) {
      continue;
    }

    uint8_t shiftedR = r >> RIGHT_SHIFT;
    uint8_t shiftedG = g >> RIGHT_SHIFT;
    uint8_t shiftedB = b >> RIGHT_SHIFT;

    rMin = std::min(rMin, shiftedR);
    gMin = std::min(gMin, shiftedG);
    bMin = std::min(bMin, shiftedB);
    rMax = std::max(rMax, shiftedR);
    gMax = std::max(gMax, shiftedG);
    bMax = std::max(bMax, shiftedB);

    int index =
        makeColorIndexOf(static_cast<int>(shiftedR), static_cast<int>(shiftedG),
                         static_cast<int>(shiftedB));

    histogram[index]++;
  }
  MMCQ::VBox vbox(rMin, rMax, gMin, gMax, bMin, bMax, histogram);
  return {histogram, vbox};
}

std::vector<MMCQ::VBox, std::allocator<MMCQ::VBox>> MMCQ::applyMedianCut(
    const std::vector<int, std::allocator<int>>& histogram, const VBox& vbox) {
  if (vbox.getCount() == 0) {
    return {};
  }

  if (vbox.getCount() == 1) {
    return {vbox};
  }

  int total = 0;
  std::vector<int> partialSum(VBOX_LENGTH, -1);
  int vboxMin;
  int vboxMax;

  ColorChannel axis = vbox.widestColorChannel();
  switch (axis) {
    case ColorChannel::R:
      vboxMin = vbox.rMin;
      vboxMax = vbox.rMax;
      for (int i = vbox.rMin; i <= vbox.rMax; i++) {
        int sum = 0;
        for (int j = vbox.gMin; j <= vbox.gMax; j++) {
          for (int k = vbox.bMin; k <= vbox.bMax; k++) {
            int index = makeColorIndexOf(i, j, k);
            sum += histogram[index];
          }
        }
        total += sum;
        partialSum[i] = total;
      }
      break;
    case ColorChannel::G:
      vboxMin = vbox.gMin;
      vboxMax = vbox.gMax;
      for (int i = vbox.gMin; i <= vbox.gMax; i++) {
        int sum = 0;
        for (int j = vbox.rMin; j <= vbox.rMax; j++) {
          for (int k = vbox.bMin; k <= vbox.bMax; k++) {
            int index = makeColorIndexOf(j, i, k);
            sum += histogram[index];
          }
        }
        total += sum;
        partialSum[i] = total;
      }
      break;
    case ColorChannel::B:
      vboxMin = vbox.bMin;
      vboxMax = vbox.bMax;
      for (int i = vbox.bMin; i <= vbox.bMax; i++) {
        int sum = 0;
        for (int j = vbox.rMin; j <= vbox.rMax; j++) {
          for (int k = vbox.gMin; k <= vbox.gMax; k++) {
            int index = makeColorIndexOf(j, k, i);
            sum += histogram[index];
          }
        }
        total += sum;
        partialSum[i] = total;
      }
      break;
  }

  std::vector<int> lookAheadSum(VBOX_LENGTH, -1);
  for (int i = vboxMin; i < vboxMax; i++) {
    int cumulativeSum = partialSum[i];
    if (cumulativeSum != -1) {
      lookAheadSum[i] = total - cumulativeSum;
    }
  }

  return cut(axis, vbox, vboxMin, vboxMax, partialSum, lookAheadSum, total);
}

std::vector<MMCQ::VBox> MMCQ::cut(ColorChannel axis, const VBox& vbox,
                                  int vboxMin, int vboxMax,
                                  const std::vector<int>& partialSum,
                                  const std::vector<int>& lookAheadSum,
                                  int total) {
  int left = vboxMin;
  int right = vboxMax;
  int splitPoint = vboxMin;

  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (partialSum[mid] <= total / 2) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  splitPoint = right;

  if (splitPoint >= vboxMin && splitPoint < vboxMax) {
    MMCQ::VBox vbox1(vbox);
    MMCQ::VBox vbox2(vbox);

    int left = splitPoint - vboxMin;
    int right = vboxMax - splitPoint;

    int d2;
    if (left <= right) {
      d2 = std::min(vboxMax - 1, splitPoint + right / 2);
    } else {
      d2 = std::max(vboxMin, splitPoint - 1 - left / 2);
    }

    while (d2 < 0 || partialSum[d2] <= 0) {
      d2++;
    }
    int rightPartitionCount = lookAheadSum[d2];
    while (rightPartitionCount == 0 && d2 > 0 && partialSum[d2 - 1] > 0) {
      d2--;
      rightPartitionCount = lookAheadSum[d2];
    }

    switch (axis) {
      case ColorChannel::R:
        vbox1.rMax = static_cast<uint8_t>(d2);
        vbox2.rMin = static_cast<uint8_t>(d2 + 1);
        break;
      case ColorChannel::G:
        vbox1.gMax = static_cast<uint8_t>(d2);
        vbox2.gMin = static_cast<uint8_t>(d2 + 1);
        break;
      case ColorChannel::B:
        vbox1.bMax = static_cast<uint8_t>(d2);
        vbox2.bMin = static_cast<uint8_t>(d2 + 1);
        break;
    }

    return {vbox1, vbox2};
  }

  return {};
}

void MMCQ::iterate(std::vector<VBox>& queue,
                   bool (*comparator)(const VBox&, const VBox&), int target,
                   const std::vector<int>& histogram) {
  int niters = 0;
  while (niters < MAX_ITERATIONS) {
    if (queue.empty()) {
      return;
    }
    VBox vbox = queue.back();

    if (vbox.getCount() == 0) {
      std::sort(queue.begin(), queue.end(), comparator);
      niters++;
      continue;
    }

    queue.pop_back();

    std::vector<VBox> vboxes = applyMedianCut(histogram, vbox);
    if (vboxes.empty()) {
      continue;
    };
    queue.push_back(vboxes[0]);
    if (vboxes.size() == 2) {
      queue.push_back(vboxes[1]);
    }

    std::sort(queue.begin(), queue.end(), comparator);

    if (queue.size() >= target) {
      return;
    };
    niters++;
  }
}

bool MMCQ::compareByCount(const VBox& a, const VBox& b) {
  return a.getCount() < b.getCount();
}

bool MMCQ::compareByProduct(const VBox& a, const VBox& b) {
  int aCount = a.getCount();
  int bCount = b.getCount();
  int aVolume = a.getVolume();
  int bVolume = b.getVolume();

  if (aCount == bCount) {
    return aVolume < bVolume;
  } else {
    int64_t aProduct =
        static_cast<int64_t>(aCount) * static_cast<int64_t>(aVolume);
    int64_t bProduct =
        static_cast<int64_t>(bCount) * static_cast<int64_t>(bVolume);
    return aProduct < bProduct;
  }
}