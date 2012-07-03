#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

#include "TempoMap.h"

#include <boost/utility.hpp>

namespace cf {
namespace ScoreFollower {

class TimeWarper;
class Follower;

class TempoFollower : public boost::noncopyable
{
public:
	TempoFollower(TimeWarper const & timeWarper, Follower & parent);

	void ReadTempoTrack(TempoReaderPtr & reader) { tempoMap_.Read(reader); }
	void RegisterBeat(real_time_t const & beatTime);

	speed_t SpeedEstimateAt(real_time_t const & time);

private:

	struct BeatClassification
	{
		BeatClassification(beat_pos_t offset, double probability)
			: offset(offset), probability(probability) {}

		beat_pos_t offset;
		double probability;
	};

	typedef EventBuffer<BeatClassification, real_time_t> BeatHistoryBuffer;

private:
	BeatClassification ClassifyBeatAt(real_time_t const & time);
	beat_pos_t BeatOffsetEstimate() const;
	tempo_t BeatLengthEstimate() const;

	speed_t SpeedFromConductedTempo(TempoPoint const & tempoNow, real_time_t const & now) const;
	speed_t SpeedFromBeatCatchup(TempoPoint const & tempoNow, beat_pos_t catchupTime) const;

private:
	TimeWarper const & timeWarper_;
	Follower & parent_;

	TempoMap tempoMap_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	speed_t speed_;
};

} // namespace ScoreFollower
} // namespace cf
