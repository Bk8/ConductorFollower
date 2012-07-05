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
	FollowerImpl(unsigned samplerate, unsigned blockSize);
	~FollowerImpl();

public: // Follower implementation
	Status::FollowerStatus & status() { return status_; }
	Options::FollowerOptions & options() { return options_; }

	void CollectData(boost::shared_ptr<ScoreReader> scoreReader);
	void StartNewBlock();
	void GetTrackEventsForBlock(unsigned track, BlockBuffer & events);

private:
	FollowerState State();
	void SetState(FollowerState::Value state);

	void CopyEventToBuffer(score_time_t const & time, ScoreEventPtr data, BlockBuffer & events) const;
	double NewVelocityAt(double oldVelocity, score_time_t const & time) const;

	void GotStartGesture(real_time_t const & beatTime, real_time_t const & startTime);
	void ConsumeEvent(MotionTracker::Event const & e);

	void HandleNewPosition(real_time_t const & timestamp);
	void UpdateMagnitude(real_time_t const & timestamp);

private:
	GlobalsInitializer globalsInit_;

	Status::FollowerStatus status_;
	Options::FollowerOptions options_;

	// Created via shared_ptr
	boost::shared_ptr<MotionTracker::EventProvider> eventProvider_;
	boost::shared_ptr<FeatureExtractor::Extractor> featureExtractor_;

	// Can't be constructed in init list
	boost::shared_ptr<MotionTracker::EventThrottler> eventThrottler_;
	boost::shared_ptr<TimeHelper> timeHelper_;
	
	ScoreHelper scoreHelper_;

	real_time_t startRollingTime_;
	double velocity_;

	typedef boost::signals2::connection SignalConnection;

	SignalConnection startGestureConnection_;
	SignalConnection beatConnection_;
};

} // namespace ScoreFollower
} // namespace cf
