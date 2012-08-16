#pragma once

#include "ScoreFollower/types.h"
#include "ScoreFollower/TimeSignature.h"

namespace cf {
namespace ScoreFollower {

class ScorePosition
{
public:
	ScorePosition() {} // Allow uninitialized ctor
	ScorePosition(score_time_t const & ref, beat_pos_t const & position,
		tempo_t const & tempo, TimeSignature const & timeSignature)
		: reference_(ref)
		, position_(position)
		, tempo_(tempo)
		, timeSignature_(timeSignature)
	{}

	beat_pos_t const & position() const { return position_; }
	tempo_t const & tempo() const { return tempo_; }
	TimeSignature const & meter() const { return timeSignature_; }

	beats_t BeatsTo(score_time_t const & time) const
	{
		return (time - reference_) * tempo_;
	}

	beat_pos_t BeatPositionAt(score_time_t const & time) const
	{
		return position_ + BeatsTo(time);
	}

private:
	score_time_t reference_;
	beat_pos_t position_;
	tempo_t tempo_;
	TimeSignature timeSignature_;
};

} // namespace ScoreFollower
} // namespace cf