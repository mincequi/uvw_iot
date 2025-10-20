#include "Filter.h"

#include <algorithm>
#include <cmath>

namespace uvw_iot::util {

void ema(int& prev, int curr, std::chrono::milliseconds deltaT, std::chrono::milliseconds tau) {
    deltaT = std::min(deltaT, tau);
    prev += round((curr - prev) * (double)deltaT.count() / (double)tau.count());
}

} // namespace uvw_iot::util
