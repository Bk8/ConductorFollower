#include "DimensionFeatureExtractor.h"

#include <iostream>

#include <boost/geometry.hpp>
#include <boost/range/numeric.hpp>

namespace bg = boost::geometry;

namespace cf {
namespace FeatureExtractor {

DimensionFeatureExtractor::DimensionFeatureExtractor(PositionBuffer const & eventBuffer)
	: positionBuffer_(eventBuffer)
{
}

void
DimensionFeatureExtractor::CalculateStuff(InterThreadEventBuffer & events)
{
	timestamp_t lastTimestamp = positionBuffer_.AllEvents().LastTimestamp();
	timestamp_t since = lastTimestamp - milliseconds_t(1000);
	auto eventsSince = positionBuffer_.EventsSince(since);
	auto range = eventsSince.DataAs<IteratorLinestring>();

	Box3D envelope;
	bg::envelope(range, envelope);
	
	Point3D distance = geometry::distance_vector(envelope.min_corner(), envelope.max_corner());
	assert(events.enqueue(Event(lastTimestamp, Event::Magnitude, distance)));
	//std::cout << "Spatial magnitude of movement: " << boost::geometry::dsv(distance) << std::endl;

	Point3D centroid;
	bg::centroid(range, centroid);
	//std::cout << "Centroid of movement: " << boost::geometry::dsv(centroid) << std::endl;

	coord_t length = bg::length(range);
	//std::cout << "Length of movement: " << length << std::endl;
}

} // namespace FeatureExtractor
} // namespace cf
