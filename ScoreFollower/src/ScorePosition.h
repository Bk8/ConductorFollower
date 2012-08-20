#pragma once

#include "cf/units_math.h"
#include "cf/TimeSignature.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class ScorePosition
{
public: // Constructors and generation methods

	// Position at beginning, with MIDI defaults
	ScorePosition(tempo_t const & tempo = tempo_t(120.0 * score::beats_per_minute),
	              TimeSignature const & meter = TimeSignature(4, 4))
		: absoluteTime_(0.0 * score::seconds)
		, absolutePosition_(0.0 * score::beats)
		, tempo_(tempo)
		, meter_(meter)
		, bar_(0.0 * score::bars)
		, beat_(0.0 * score::beats)
	{}

	ScorePosition ChangeAt(score_time_t const & time, TimeSignature const & meter, tempo_t const & tempo) const
	{
		auto barDuration = meter.BarDuration();
		bars_t barsFromBeginningOfThisBar((BeatsTo(time) + beat_) / barDuration);
		bars_t wholeBars = boost::units::floor(barsFromBeginningOfThisBar);
		beats_t beat((barsFromBeginningOfThisBar - wholeBars) * barDuration);
		beat_pos_t position = AbsolutePositionAt(time);

		return ScorePosition(time, position, tempo, meter, bar_ + wholeBars, beat);
	}

	ScorePosition ChangeAt(score_time_t const & time, tempo_t const & tempo, TimeSignature const & meter) const
	{
		return ChangeAt(time, meter, tempo);
	}

	ScorePosition ChangeAt(score_time_t const & time, tempo_t const & tempo) const
	{
		return ChangeAt(time, meter_, tempo);
	}

	ScorePosition ChangeAt(score_time_t const & time, TimeSignature const & meter) const
	{
		return ChangeAt(time, meter, tempo_);
	}

	ScorePosition ScorePositionAt(score_time_t const & time) const
	{
		return ChangeAt(time, meter_, tempo_);
	}

	ScorePosition ScorePositionAt(beat_pos_t const & absolutePosition) const
	{
		// We only go toward the future
		assert(absolutePosition >= absolutePosition_);

		auto barDuration = meter_.BarDuration();
		beat_pos_t beatsFromBeginningOfThisBar = absolutePosition - BeginningOfThisBar();
		bars_t barsFromBeginningOfThisBar = boost::units::floor(beatsFromBeginningOfThisBar / barDuration);

		auto time = AbsoluteTimeAt(absolutePosition);
		bars_t bar = bar_ + barsFromBeginningOfThisBar;
		beats_t beat = beatsFromBeginningOfThisBar - (barsFromBeginningOfThisBar * barDuration);

		return ScorePosition(time, absolutePosition, tempo_, meter_, bar, beat);
	}

public:

	score_time_t const & time() const { return absoluteTime_; }
	beat_pos_t const & position() const { return absolutePosition_; }
	tempo_t const & tempo() const { return tempo_; }
	TimeSignature const & meter() const { return meter_; }
	bar_pos_t const & bar() const { return bar_; }
	beat_pos_t const & beat() const { return beat_; }

	beat_pos_t BeginningOfThisBar() const
	{
		return absolutePosition_ - beat_;
	}

	beat_pos_t BeginningOfNextBar() const
	{
		return BeginningOfThisBar() + (meter_.BarDuration() * score::bar);
	}

private: // "explicit" construction is private, use the generation methods otherwise
	ScorePosition(score_time_t const & time, beat_pos_t const & position,
	              tempo_t const & tempo, TimeSignature const & meter,
	              bar_pos_t bar, beat_pos_t beat)
		: absoluteTime_(time)
		, absolutePosition_(position)
		, tempo_(tempo)
		, meter_(meter)
		, bar_(bar)
		, beat_(beat)
	{}

	beats_t BeatsTo(score_time_t const & time) const
	{
		return (time - absoluteTime_) * tempo_;
	}

	beat_pos_t AbsolutePositionAt(score_time_t const & time) const
	{
		return absolutePosition_ + BeatsTo(time);
	}

	score_time_t AbsoluteTimeAt(beat_pos_t const & absolutePosition) const
	{
		beats_t beatDiff = absolutePosition - absolutePosition_;
		return absoluteTime_ + (beatDiff / tempo_);
	}

private:
	score_time_t absoluteTime_;
	beat_pos_t absolutePosition_;
	tempo_t tempo_;
	TimeSignature meter_;
	bar_pos_t bar_;
	beat_pos_t beat_;
};

} // namespace ScoreFollower
} // namespace cf