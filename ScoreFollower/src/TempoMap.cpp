#include "TempoMap.h"

namespace cf {
namespace ScoreFollower {

TempoMap::TempoMap()
	: changes_(16)
{
}

void
TempoMap::Read(TrackReader<tempo_t> & reader)
{
	bool first = true;
	TempoChange previousChange;
	score_time_t timestamp;
	tempo_t tempo;

	while (reader.NextEvent(timestamp, tempo)) {
		beat_pos_t pos = first ? 0.0 : previousChange.GetTempoAt(timestamp).position;
		first = false;

		previousChange = TempoChange(timestamp, TempoPoint(pos, tempo));
		changes_.RegisterEvent(timestamp, previousChange);
	}

	EnsureChangesNotEmpty();
}

TempoMap::TempoPoint
TempoMap::GetTempoAt(score_time_t const & time)
{
	auto range = changes_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range.data().GetTempoAt(time);
}

void
TempoMap::EnsureChangesNotEmpty()
{
	if (changes_.AllEvents().Empty()) {
		seconds_t qDuration(60.0/*s*/ / 120 /*bpm*/);
		TempoPoint tp(0.0, time::duration_cast<tempo_t>(qDuration));
		changes_.RegisterEvent(score_time_t::zero(), TempoChange(score_time_t::zero(), tp));
	}
}

/****** TempoChange ******/

TempoMap::TempoChange::TempoChange(score_time_t const & timestamp, TempoPoint const & tempo)
	: timestamp_(timestamp)
	, tempo_(tempo)
{
}

TempoMap::TempoPoint
TempoMap::TempoChange::GetTempoAt(score_time_t const & time) const
{
	score_time_t modulo = time % tempo_.tempo;
	score_time_t::rep beats = time / tempo_.tempo;
	beat_pos_t fraction = static_cast<beat_pos_t>(modulo.count()) / tempo_.tempo.count();
	beat_pos_t position = tempo_.position + beats + fraction;
	return TempoPoint(position, tempo_.tempo);
}

} // namespace ScoreFollower
} // namespace cf
