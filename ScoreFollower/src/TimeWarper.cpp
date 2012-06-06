#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

TimeWarper::WarpPoint::WarpPoint(speed_t speed, real_time_t realTime, score_time_t scoreTime)
	: speed_(speed)
	, realTime_(realTime)
	, scoreTime_(scoreTime)
{
}

score_time_t
TimeWarper::WarpPoint::Warp(real_time_t const & time) const
{
	// Only allow warping after the fixed point
	assert(time >= realTime_);

	duration_t diff = time - realTime_;
	duration_t warpedDuration(static_cast<duration_t::rep>(diff.count() * speed_));

	score_time_t progress = boost::chrono::duration_cast<score_time_t>(warpedDuration);
	return scoreTime_ + progress;
}


real_time_t
TimeWarper::WarpPoint::InverseWarp(score_time_t const & time) const
{
	// Only allow warping after the fixed point
	assert(time >= scoreTime_);

	duration_t diff = time - scoreTime_;
	duration_t warpedDuration(static_cast<duration_t::rep>(diff.count() * (1.0 / speed_)));

	score_time_t progress = boost::chrono::duration_cast<score_time_t>(warpedDuration);
	return realTime_ + progress;
}

TimeWarper::TimeWarper()
	: tempoMap_(0)
	, warpHistory_(100) // Arbitrary length, should be long enough...
	, beatHistory_(100) // Arbitrary length, should be long enough...
{
}

void
TimeWarper::ReadTempoTrack(TrackReader<tempo_t> & reader)
{
	score_time_t timestamp;
	tempo_t tempo;
	while (reader.NextEvent(timestamp, tempo)) {
		tempoMap_.RegisterEvent(timestamp, tempo);
	}

	if (tempoMap_.AllEvents().Empty()) {
		seconds_t qDuration(60.0/*s*/ / 120 /*bpm*/);
		tempoMap_.RegisterEvent(score_time_t::zero(), time::duration_cast<tempo_t>(qDuration));
	}
}

void
TimeWarper::RegisterBeat(real_time_t const & beatTime)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());

	// Beats will probably include a probability later...
	beatHistory_.RegisterEvent(beatTime, 1.0);
}

void
TimeWarper::FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime)
{
	speed_t speed = CalculateSpeedAt(realTime);
	warpHistory_.RegisterEvent(realTime, WarpPoint(speed, realTime, scoreTime));
}

score_time_t
TimeWarper::WarpTimestamp(real_time_t const & time)
{
	assert(time >= warpHistory_.AllEvents().LastTimestamp());

	auto history = warpHistory_.EventsSinceInclusive(time);
	if (history.Empty()) {
		return score_time_t::zero();
	} else {
		return history.data().Warp(time);
	}
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time, real_time_t const & reference)
{
	auto history = warpHistory_.EventsSinceInclusive(reference);
	return InverseWarpTimestamp(time, history);
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time)
{
	auto history = warpHistory_.AllEvents();
	return InverseWarpTimestamp(time, history);
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time, WarpHistoryBuffer::Range & searchRange)
{
	assert(!searchRange.Empty());
	assert(time >= searchRange.data().scoreTime());

	while (!searchRange.AtEnd()) {
		WarpPoint const & p = searchRange.data();

		// Check the next one (if it exists)
		if (searchRange.Next() && (time >= searchRange.data().scoreTime())) { continue; }
		return p.InverseWarp(time);
	}

	throw std::logic_error("Should never be reached...");
}

TimeWarper::speed_t
TimeWarper::CalculateSpeedAt(real_time_t time)
{
	// Ignore beginning for now...
	if (beatHistory_.AllEvents().Size() < 16) { return 1.0; }
	
	speed_t speed = 1.0;
	
	score_time_t scoreTime = WarpTimestamp(time);
	auto lastTempoEvent = tempoMap_.EventsSinceInclusive(scoreTime);
	auto lastMeasure = beatHistory_.LastNumEvnets(3);

	real_time_t::duration lmDuration = lastMeasure.timestampRange().back() - lastMeasure.timestampRange().front();
	tempo_t cTempo = time::duration_cast<tempo_t>(lmDuration / 2);

	// tempos are durations, so the division is this way around...
	speed = (double)lastTempoEvent.data().count() / (double)cTempo.count();

	return speed;
}


} // namespace ScoreFollower
} // namespace cf
