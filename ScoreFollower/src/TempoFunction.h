#pragma once

#include "cf/units_math.h"

#include "ScoreFollower/types.h"

#include <boost/utility.hpp>

namespace cf {
namespace ScoreFollower {

class TempoFunction : public boost::noncopyable
{
public:
	TempoFunction();

	void SetParameters(
		real_time_t const & startTime, time_quantity const & changeTime,
		tempo_t const & startTempo, tempo_t const & tempoChange,
		beat_pos_t const & cathcup);
	void SetConstantTempo(tempo_t const & tempo);

	tempo_t TempoAt(real_time_t const & time) const;
	beat_pos_t CatchupAt(real_time_t const & time) const;

private:
	tempo_t LinearTempoChangeAfter(time_quantity const & time) const;
	tempo_t NonLinearTempoChangeAfter(time_quantity const & time) const;

	beat_pos_t LinearCatchupAfter(time_quantity const & time) const;
	beat_pos_t NonLinearCatchupAfter(time_quantity const & time) const;

private:
	real_time_t startTime_;
	time_quantity changeTime_;
	
	tempo_t startTempo_;

	tempo_change_t linearTempoChange_;
	double nonLinearCoef_;

private:
	static const double pi;
};

} // namespace ScoreFollower
} // namespace cf
