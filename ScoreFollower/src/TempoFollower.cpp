#include "TempoFollower.h"

#include <boost/bind.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "cf/globals.h"
#include "cf/math.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

namespace si = boost::units::si;

class TempoFollower::ScoreEventBuilder
{
public:
	typedef void result_type;

	ScoreEventBuilder(TempoFollower & parent)
		: parent_(parent) {}

	void operator() (Data::TempoSensitivityChange const & change) const
	{
		auto scoreTime = parent_.tempoMap_.TimeAt(change.position);
		parent_.tempoSensitivities_.RegisterEvent(scoreTime, change);
	}

	void operator() (Data::Fermata const & fermata) const
	{
		Fermata f(fermata, parent_.tempoMap_);

		if (!parent_.fermatas_.AllEvents().Empty()) {
			assert(parent_.fermatas_.AllEvents().Back().data.end < f.tempoReference);
		}

		parent_.fermatas_.RegisterEvent(f.tempoReference.time(), f);
	}

private:
	TempoFollower & parent_;
};

TempoFollower::TempoFollower(TimeWarper const & timeWarper, Follower & parent)
	: timeWarper_(timeWarper)
	, parent_(parent)
	, tempoMap_()
	, startTempoEstimator_()
	, beatClassifier_(tempoMap_)
	, beatHistory_(100) // Arbitrary length, should be long enough...
	, newBeats_(false)
{
}

void
TempoFollower::ReadScore(ScoreReader & reader)
{
	tempoMap_.ReadScore(reader);
	auto start = tempoMap_.GetScorePositionAt(0.0 * score::seconds);
	startTempoEstimator_.SetStartTempo(start.tempo());
}

void
TempoFollower::LearnScoreEvents(Data::ScoreEventList const & events)
{
	tempoSensitivities_.Clear();
	fermatas_.Clear();

	// Collect all events
	ScoreEventBuilder visitor(*this);
	std::for_each(std::begin(events), std::end(events),
		[visitor](Data::ScoreEvent const & event)
		{
			boost::apply_visitor(visitor, event);
		});

	// Preparations for beginning, catch fermatas at 0 with negative value
	LookupNextFermata(-1.0 * score::seconds);
}

void
TempoFollower::RegisterStartGesture(MotionTracker::StartGestureData const & data)
{
	startTempoEstimator_.RegisterStartGesture(data);
	auto speed = startTempoEstimator_.SpeedEstimate();
	acceleration_.SetConstantSpeed(speed);
}

void
TempoFollower::RegisterBeat(real_time_t const & beatTime, double clarity)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());

	auto classification = ClassifyBeatAt(beatTime, clarity);
	if (true /* TODO rejection! */) {
		beatHistory_.RegisterEvent(beatTime, classification);
		LOG("Classified with offset: %1%", classification.offset());
		newBeats_ = true;
	}
}


speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	score_time_t scoreTime = timeWarper_.WarpTimestamp(time);

	// Beats
	if (newBeats_) {
		newBeats_ = false;

		tempo_t tempoNow = tempoMap_.GetScorePositionAt(scoreTime).tempo();

		// TODO Move to estimator
		auto offsetEstimate = -BeatOffsetEstimate();
		auto accelerationTime = AccelerationTimeAt(scoreTime);
		auto tempoChange = offsetEstimate / boost::units::pow<2>(accelerationTime);
		SpeedFunction::SpeedChangeRate acceleration(tempoChange / tempoNow);

		//LOG("Beat offset: %1%, tempo change: %2%, acceleartion: %3%",
		//	offsetEstimate, tempoChange.value(), acceleration.value());
		
		// TODO clean up
		if (nextFermata_.IsInFermata()) {
			acceleration_.SetConstantSpeed(fermataReferenceSpeed_);
			nextFermata_.Reset();
		} else {
			auto speed = acceleration_.SpeedAt(time);
			acceleration_.SetParameters(speed, time, acceleration, accelerationTime);
		}
	}

	// Fermata
	switch (nextFermata_.GetActionAtTime(scoreTime))
	{
	case FermataState::StoreReferenceTempo:
		fermataReferenceSpeed_ = acceleration_.SpeedAt(time);
		break;
	case FermataState::EnterFermata:
		acceleration_.SetConstantSpeed(0.0);
		break;
	}

	return acceleration_.SpeedAt(time);
}

BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time, double clarity)
{
	// TODO clean up
	if (nextFermata_.IsInFermata()) {
		return beatClassifier_.ClassifyBeat(nextFermata_.FermataEnd(), 1.0);
	}

	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	ScorePosition position = tempoMap_.GetScorePositionAt(beatScoreTime);

	LOG("Got beat at: %1% | %2%", position.bar(), position.beat());
	return beatClassifier_.ClassifyBeat(position, acceleration_.FractionAt(time));
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	auto lastBeat = beatHistory_.AllEvents().Back().data;
	return lastBeat.offset();
}

score_time_t
TempoFollower::AccelerationTimeAt(score_time_t time)
{
	// first beats can be before 0, but we want the sensitivity from the beginning
	time = boost::units::max(time, 0.0 * score::seconds);

	score_time_t const minAccelerationTime = time_quantity(1.0 * score::seconds);
	score_time_t const accelerationTimeRange = time_quantity(1.6 * score::seconds);
	score_time_t const defaultAccelerationTime = time_quantity(1.8 * score::seconds);

	auto changes = tempoSensitivities_.EventsSinceInclusive(time);
	if (changes.Empty()) { return defaultAccelerationTime; }

	auto sensitivity = changes.Front().data.sensitivity;
	auto factor = 1.0 - sensitivity;
	return minAccelerationTime + (factor * accelerationTimeRange);
}

void
TempoFollower::LookupNextFermata(score_time_t const & timeNow)
{
	auto fermatas = fermatas_.EventsSince(timeNow);
	if (fermatas.Empty()) { return; }

	nextFermata_ = fermatas.Front().data;
}

} // namespace ScoreFollower
} // namespace cf
