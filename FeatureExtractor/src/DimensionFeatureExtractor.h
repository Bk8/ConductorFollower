#pragma once

#include <boost/utility.hpp>

#include "cf/cf.h"

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class DimensionFeatureExtractor : public boost::noncopyable
{
public:
	DimensionFeatureExtractor(EventBuffer<Point3D, timestamp_t> const & eventBuffer);

	void Update();

	Point3D MagnitudeSince(timestamp_t const & time);

private:
	EventBuffer<Point3D, timestamp_t> const & positionBuffer_;
};

} // namespace FeatureExtractor
} // namespace cf
