#pragma once

#include <boost/variant.hpp>

#include "cf/cf.h"

namespace cf {
namespace FeatureExtractor {

class Event
{
public:
	// Types
	enum Type
	{
		Invalid = -1,
		TrackingStarted,
		TrackingEnded,
		Beat,
		Magnitude
	};

	// Data
	typedef boost::variant<int, double> Data;

	// Ctors
	Event() : type_(Invalid) {}

	Event(timestamp_t const & timestamp, Type type)
		: timestamp_(timestamp)
		, type_(type)
	{
		assert(type != Magnitude);
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

} // namespace FeatureExtractor
} // namespace cf
