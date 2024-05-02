#include "Util.h"

namespace uvw_iot {
namespace common {

int Util::divAndRound(int dividend, int divisor) {
    return (dividend >= 0) ? (dividend + divisor/2)/divisor : (dividend - divisor/2)/divisor;
}

} // namespace common
} // namespace uvw_iot
