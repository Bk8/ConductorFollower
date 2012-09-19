#pragma once

#include <boost/variant.hpp>

#include "cf/time.h"

#include "Visualizer/Data.h"

#include "MotionTracker/MotionState.h"
#include "MotionTracker/StartGestureData.h"

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
		MotionStateUpdate,
		Beat,
		BeatProb,
		StartGesture,
		VelocityPeak,
		VelocityDynamicRange,
		JerkPeak,
		VisualizationData
	};

	// Data
	typedef boost::variant<
		boost::blank,
		double,
		MotionState,
		StartGestureData,
		timestamp_t, duration_t,
		Visualizer::DataPtr
		> Data;

	// Ctors
	Event() : type_(Invalid) {}

	Event(timestamp_t const & timestamp, Type type)
		: timestamp_(timestamp)
		, type_(type)
	{
		// Assert that a position event is not constructed without data
		assert(type != MotionStateUpdate);
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
