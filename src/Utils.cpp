#include "Utils.h"

namespace utils {
TimeStamp::TimeStamp() { this->saved = clock_t::now(); }
void TimeStamp::reset() { this->saved = clock_t::now(); }
long long TimeStamp::getElapsedMs() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               clock_t::now() - saved)
        .count();
}
long long TimeStamp::getElapsedMs(TimeStamp const& o) const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(o.saved -
                                                                 saved)
        .count();
}
bool TimeStamp::isOlderThanMs(long long ms) const {
    return getElapsedMs() > ms;
}

TimeStamp::time_point_t TimeStamp::getSavedTime() const { return this->saved; }

bool TimeStamp::resetIfOlderThanMs(long long ms) {
    if (isOlderThanMs(ms)) {
        reset();
        return true;
    }
    return false;
}

}  // namespace utils