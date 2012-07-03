#pragma once

#include <vector>

#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "cf/EventBuffer.h"

namespace cf {
namespace ScoreFollower {

class ScoreEventHandle;
class ScoreEventManipulator;
class ScoreReader;

namespace Status { class FollowerStatus; }
namespace Options { class FollowerOptions; }

class Follower : public boost::noncopyable
{
public:
	// Container for fetching events for each block
	typedef EventBuffer<ScoreEventHandle, unsigned> BlockBuffer;

public:
	// Implementation is hidde behind the factory function
	static boost::shared_ptr<Follower> Create(unsigned samplerate, unsigned blockSize);
	virtual ~Follower() {}

	virtual Status::FollowerStatus & status() = 0;
	virtual Options::FollowerOptions & options() = 0;

	// Collect data from scoreReader and keep it in scope as long as the data is used.
	virtual void CollectData(boost::shared_ptr<ScoreReader> scoreReader) = 0;

	// Start new audio block (blocksize defined in ctor)
	virtual void StartNewBlock() = 0;

	// Gets events for track in current block, using the given manipulator
	virtual void GetTrackEventsForBlock(unsigned track, ScoreEventManipulator & manipulator, BlockBuffer & events) = 0;
};

} // namespace ScoreFollower
} // namespace cf
