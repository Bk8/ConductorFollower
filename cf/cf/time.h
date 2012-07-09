#pragma once

#include <boost/chrono.hpp>

namespace cf {

typedef boost::chrono::steady_clock::time_point timestamp_t;
typedef boost::chrono::steady_clock::duration duration_t;
typedef boost::chrono::milliseconds milliseconds_t;
typedef boost::chrono::microseconds microseconds_t;
typedef boost::chrono::duration<double> seconds_t;

namespace time {

inline timestamp_t now() { return boost::chrono::steady_clock::now(); }

template<typename T, typename Y> T duration_cast(Y y) { return boost::chrono::duration_cast<T>(y); }

template<typename TTime, typename TMultiplier>
TTime multiply(TTime const & time, TMultiplier const & multiplier)
{
	TMultiplier result = multiplier * time.count();
	return TTime(static_cast<TTime::rep>(result));
}

template<typename TTime, typename TDivisor>
TTime divide(TTime const & time, TDivisor const & divisor)
{
	TDivisor result = time.count() / divisor;
	return TTime(static_cast<TTime::rep>(result));
}

template<typename TTime, typename TDuration>
void limitRange(TTime & time, TTime const & min, TTime const & max, TDuration assertThreshold = TDuration::zero())
{
	if (time < min) {
		assert(duration_cast<TDuration>(min - time) < assertThreshold);
		time = min;
	} else if (time > max) {
		assert(duration_cast<TDuration>(time - max) < assertThreshold);
		time = max;
	}
}

} // namespace time

} // namespace cf
