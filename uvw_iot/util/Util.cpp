#include "Util.h"

namespace uvw_iot {
namespace util {

int divAndRound(int dividend, int divisor) {
    return (dividend >= 0) ? (dividend + divisor/2)/divisor : (dividend - divisor/2)/divisor;
}

} // namespace util
} // namespace uvw_iot
