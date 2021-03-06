#include "BeatPattern.h"

#include "cf/algorithm.h"
#include "cf/globals.h"

#include "BeatClassification.h"

namespace cf {
namespace ScoreFollower {

/* MatchResult */

BeatPattern::MatchResult::MatchResult(BeatPattern const & parent, beat_array const & beats, double scale)
	: parent_(parent)
	, scale_(scale)
{
	assert(!parent_.scorers_.empty());
	assert(!beats.empty());
	pivot_ = beats.front();

	beat_array scaled;
	std::for_each(std::begin(beats), std::end(beats),
		[&](beat_pos_t const & beat)
		{
			scaled.push_back(ScaleWithPivot(beat));
		});

	IterationHelper helper(parent);
	std::for_each(std::begin(scaled), std::end(scaled),
		[&](beat_pos_t const & beat)
		{
			helper.Advance(beat);
		});

	quality_ = helper.score;
}

beat_pos_t
BeatPattern::MatchResult::ScaleWithPivot(beat_pos_t pos) const
{
	auto distanceFromPivot = pos - pivot_;
	return pivot_ + scale_ * distanceFromPivot;
}


beat_pos_t
BeatPattern::MatchResult::OffsetToBest(beat_pos_t const & pos) const
{
	auto scaled = ScaleWithPivot(pos);
	IterationHelper helper(parent_);
	helper.Advance(scaled);
	return helper.it->OffsetTo(pos - helper.barOffset);
}

bool
BeatPattern::MatchResult::IsConfident(BeatClassification const & classification) const
{
	auto pos = classification.position().beat();
	auto scaled = ScaleWithPivot(pos);
	IterationHelper helper(parent_);
	helper.Advance(scaled);

	auto best = helper.it;
	auto next = best, prev = best;
	parent_.WrappingAdvance(prev, -1);
	parent_.WrappingAdvance(next, 1);

	// Assumes all scores are negative or zero!
	double factor = 0.8;
	return (best->ScoreForBeat(scaled) > factor * next->ScoreForBeat(scaled)) &&
	       (best->ScoreForBeat(scaled) > factor * prev->ScoreForBeat(scaled));
}

/* Beat Pattern */

BeatPattern::BeatPattern(Data::BeatPattern const & pattern, Follower::OptionsBuffer::Reader & optionsReader)
	: optionsReader_(optionsReader)
	, meter_(pattern.meter)
{
	for (int i = 0; i < pattern.beats.size(); ++i) {
		scorers_.push_back(BeatScorer(pattern, i));
	}
}

BeatPattern::MatchResult
BeatPattern::Match(beat_array const & beats, double scale) const
{
	return MatchResult(*this, beats, scale);
}

void
BeatPattern::IterationHelper::Advance(beat_pos_t const & pos)
{
	auto initial = it;
	// penalty factor for missing or extra beats
	auto anomalyFactor = parent.optionsReader_->at<Options::BeatAnomalyCoef>();

	while (true) {
		if (it == parent.scorers_.end()) {
			it = parent.scorers_.begin();
			barOffset += parent.meter_.BarDuration() * score::bar;
		}
		auto offsetPos = pos - barOffset;

		auto next = it;
		std::advance(next, 1);
		auto nextOffsetPos = pos - barOffset;
		if (next == parent.scorers_.end()) {
			nextOffsetPos -= parent.meter_.BarDuration() * score::bar;
			next = parent.scorers_.begin();
		}

		auto curScore = it->ScoreForBeat(offsetPos);
		auto nextScore = next->ScoreForBeat(nextOffsetPos);

		if (nextScore < curScore) {
			if (it == initial && !first) {
				// duplicate classification
				score += anomalyFactor * it->PenaltyForUsed();
			}

			// we found the best one
			score += curScore;
			break;
		}

		if (it != initial && !first) {
			// We have skipped one
			score += anomalyFactor * it->PenaltyForMissed();
			//std::cout << "skip penalty" << std::endl;
		}

		++it;
	}

	first = false;
}

} // namespace ScoreFollower
} // namespace cf
