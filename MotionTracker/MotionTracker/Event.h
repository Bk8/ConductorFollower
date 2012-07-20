#pragma once

#include <boost/variant.hpp>

#include "cf/time.h"
#include "cf/geometry.h"

namespace cf {
namespace MotionTracker {

class Event
{
public:
	// Types
	enum Type
	{
		Invalid = -1,
		TrackingStarted,
		TrackingEnded,
		Position,
		Velocity,
		Acceleration,
		Jerk
	};

	// Data
	typedef boost::variant<
		Point3D,
		Velocity3D,
		Acceleration3D
		//, Jerk3D
		> Data;

	// Ctors
	Event() : type_(Invalid) {}

	Event(timestamp_t const & timestamp, Type type)
		: timestamp_(timestamp)
		, type_(type)
	{
		// Assert that a position event is not constructed without data
		assert(type != Position);
	}

	template<typename TData>
	Event(timestamp_t const & timestamp, Type type, TData data)
		: timestamp_(timestamp)
		, type_(type)
		, data_(data)
	{
	}

	// Data access
	timestamp_t timestamp() const { return timestamp_; }

	Type type() const { return type_; }

	Data data() const { return data_; }
	template <typename T> T const & data() const { return boost::get<T const &>(data_); }

private:
	timestamp_t timestamp_;
	Type type_;
	Data data_;
};

} // namespace MotionTracker
} // namespace cf
