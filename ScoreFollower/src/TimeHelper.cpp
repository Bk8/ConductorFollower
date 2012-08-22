#include "TimeHelper.h"

#include "cf/globals.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

namespace cf {
namespace ScoreFollower {

TimeHelper::TimeHelper(Follower & parent, PatchMapper::ConductorContext & conductorContext)
	: parent_(parent)
	, conductorContext_(conductorContext)
	, tempoFollower_(timeWarper_, parent) 
{
}

boost::shared_ptr<TimeHelper>
TimeHelper::FreshClone()
{
	auto clone = boost::make_shared<TimeHelper>(parent_, conductorContext_);
	clone->timeManager_ = timeManager_;
	return clone;
}

void
TimeHelper::SetBlockParameters(unsigned samplerate, unsigned blockSize)
{
	timeManager_ = boost::make_shared<AudioBlockTimeManager>(
		samplerate * score::samples_per_second,
		blockSize * score::samples);
}

void
TimeHelper::StartNewBlock()
{
	rtRange_ = timeManager_->GetRangeForNow();
}

void
TimeHelper::FixScoreRange(Follower::StatusRCU::WriterHandle & statusWriter)
{
	std::pair<score_time_t, score_time_t> scoreRange;

	// Get start estimate based on old data
	scoreRange.first = timeWarper_.WarpTimestamp(rtRange_.first);

	// Fix the starting point, ensures the next warp is "accurate"
	speed_t speed = tempoFollower_.SpeedEstimateAt(rtRange_.first);
	if (speed != previousSpeed_) {
		previousSpeed_ = speed;
		statusWriter->SetValue<Status::Speed>(speed);
		conductorContext_.tempo = speed;
		timeWarper_.FixTimeMapping(rtRange_.first, scoreRange.first, speed);
	}

	// Now use the new estimate for this block
	scoreRange.second = timeWarper_.WarpTimestamp(rtRange_.second);

	if (scoreRange.first != 0.0 * score::seconds) {
		assert(scoreRange_.second == scoreRange.first);
	}
	scoreRange_ = scoreRange;
}

void
TimeHelper::RegisterStartGestureLength(duration_t const & gestureDuration)
{
	tempoFollower_.RegisterStartGestureLength(gestureDuration);
}

void
TimeHelper::RegisterPreparatoryBeat(real_time_t const & time)
{
	tempoFollower_.RegisterPreparatoryBeat(time);
}

void
TimeHelper::RegisterBeat(real_time_t const & time, double prob)
{
	assert(time < timeManager_->CurrentBlockStart());
	tempoFollower_.RegisterBeat(time, prob);
}

samples_t
TimeHelper::ScoreTimeToFrameOffset(score_time_t const & time) const
{
	real_time_t const & ref = timeManager_->CurrentBlockStart();
	real_time_t realTime = timeWarper_.InverseWarpTimestamp(time, ref);

	// There are rounding errors sometimes, so allow 10us of "jitter" here
	// This is perfectly acceptable :)
	time::limitRange(realTime,
		timeManager_->CurrentBlockStart(), timeManager_->CurrentBlockEnd(),
		boost::chrono::microseconds(10));

	return timeManager_->ToSampleOffset(realTime);
}

} // namespace ScoreFollower
} // namespace cf