#pragma once

#include <chrono>

namespace uvw_iot::util {

void ema(int& prev, int curr, std::chrono::milliseconds deltaT, std::chrono::milliseconds tau);

} // namespace uvw_iot::util
