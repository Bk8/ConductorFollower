#pragma once

#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "cf/Logger.h"

#include "MotionTracker/Event.h"
#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerOptions.h"
#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/ScoreReader.h"

#include "AudioBlockTimeManager.h"
#include "globals.h"
#include "ScoreHelper.h"
#include "TimeHelper.h"
#include "TimeWarper.h"
#include "TempoFollower.h"

namespace cf {

namespace MotionTracker {
	class EventProvider;
	class EventThrottler;
} // namespace MotionTracker 

namespace ScoreFollower {

class FollowerImpl : public Follower
{
public:
	FollowerImpl(unsigned samplerate, unsigned blockSize, boost::shared_ptr<ScoreReader> scoreReader);
	~FollowerImpl();

public: // Follower implementation
	StatusRCU & status() { return status_; }
	OptionsRCU & options() { return options_; }

	void CollectData(boost::shared_ptr<ScoreReader> scoreReader);
	unsigned StartNewBlock();
	void GetTrackEventsForBlock(unsigned track, BlockBuffer & events);

private:
	FollowerState State();
	void SetState(FollowerState::Value state);

	void GotStartGesture(real_time_t const & beatTime, real_time_t const & startTime);
	void ConsumeEvent(MotionTracker::Event const & e);

	void HandleNewPosition(real_time_t const & timestamp);
	void UpdateMagnitude(real_time_t const & timestamp);

private:
	GlobalsInitializer globalsInit_;

	StatusRCU status_;
	OptionsRCU options_;
	FollowerState state_;

	// Created via shared_ptr
	boost::shared_ptr<ScoreReader> scoreReader_;
	boost::shared_ptr<MotionTracker::EventProvider> eventProvider_;
	boost::shared_ptr<FeatureExtractor::Extractor> featureExtractor_;

	// Can't be constructed in init list
	boost::shared_ptr<MotionTracker::EventThrottler> eventThrottler_;
	boost::shared_ptr<TimeHelper> timeHelper_;
	
	boost::shared_ptr<ScoreHelper> scoreHelper_;

	real_time_t startRollingTime_;

	typedef boost::signals2::connection SignalConnection;

	SignalConnection startGestureConnection_;
	SignalConnection beatConnection_;
};

} // namespace ScoreFollower
} // namespace cf
