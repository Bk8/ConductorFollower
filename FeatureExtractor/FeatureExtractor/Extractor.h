#pragma once

#include <vector>

#include <boost/signals2.hpp>
#include <boost/signals2/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "cf/EventBuffer.h"
#include "cf/geometry.h"
#include "cf/time.h"

namespace cf {
namespace FeatureExtractor {

namespace bs2 = boost::signals2;

typedef EventBuffer<double, timestamp_t> GestureBuffer;

class Extractor : public boost::noncopyable
{
public:
	// Hide implementation
	static boost::shared_ptr<Extractor> Create();

public: // types
	typedef bs2::keywords::mutex_type<bs2::dummy_mutex> DummyMutex;

	typedef bs2::signal_type<void (timestamp_t const &), DummyMutex>::type EventSignal;

	typedef bs2::signal_type<void (timestamp_t const & /* down beat time */,
	                               timestamp_t const & /* estimated start time */),
	                         DummyMutex>::type StartGestureSignal;

public:
	virtual ~Extractor() {}

	virtual void RegisterPosition(timestamp_t const & time, Point3D const & pos) = 0;
	virtual Point3D MagnitudeOfMovementSince(timestamp_t const & time) = 0;
	virtual Velocity3D AverageVelocitySince(timestamp_t const & time) = 0;

	virtual void EnvelopesForTimespans(Box3D & total, std::vector<Box3D> & segments, std::vector<timespan_t> const & times) = 0;
	virtual void AverageVelocityForTimespans(Velocity3D & total, std::vector<Velocity3D> & segments, std::vector<timespan_t> const & times) = 0;

	EventSignal BeatDetected;
	EventSignal ApexDetected;
	StartGestureSignal StartGestureDetected;
};

} // namespace FeatureExtractor
} // namespace cf
