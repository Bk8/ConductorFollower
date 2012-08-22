#include "BeatClassifier.h"

#include "Data/BeatPatternParser.h"

#include "BeatClassification.h"
#include "ScorePosition.h"

namespace cf {
namespace ScoreFollower {

using namespace Data;

BeatClassifier::BeatClassifier(TempoMap const & tempoMap)
	: tempoMap_(tempoMap)
{
}

void
BeatClassifier::LearnPatterns(Data::PatternMap const & patternGroups)
{
	estimators_.clear();
	int totalPatterns = 0;

	for (auto gIt = patternGroups.begin(); gIt != patternGroups.end(); ++gIt) {
		auto const & patterns = gIt->second;
		for (auto pIt = patterns.begin(); pIt != patterns.end(); ++pIt) {
			AddEstimatorsForPattern(*pIt);
			totalPatterns++;
		}
	}

	currentTimeSignature_ = tempoMap_.GetMeterAt(0.0 * score::seconds);
	currentBarStart_ = tempoMap_.GetScorePositionAt(0.0 * score::seconds);
	nextBarStart_ = tempoMap_.GetScorePositionAt(currentBarStart_.BeginningOfNextBar());
	currentOffsetEstimate_ = 0.0 * score::beats;

	LOG("Using a total of %1% beat patterns for %2% different time signatures",
		totalPatterns, (int)patternGroups.size());
}

BeatClassification
BeatClassifier::ClassifyBeat(ScorePosition const & position, double catchupFraction)
{
	BeatClassification bestClassification(position);

	auto range = estimators_.equal_range(currentTimeSignature_);
	for (auto it = range.first; it != range.second; ++it) {
		auto offset = (1.0 - catchupFraction) * currentOffsetEstimate_;
		BeatClassification classification =
			it->second.ClassifyBeat(position, currentBarStart_.position(), offset);
		if (classification.quality() > bestClassification.quality()) {
			bestClassification = classification;
		}
	}

	switch (bestClassification.type())
	{
	case BeatClassification::NextBar:
		ProgressToNextBar();
		bestClassification = ClassifyBeat(position, catchupFraction);
		break;
	case BeatClassification::CurrentBar:
		currentOffsetEstimate_ = bestClassification.offset();
		LOG("Offset estimate: %1%", currentOffsetEstimate_);
		break;
	case BeatClassification::NotClassified:
		LOG("-Dropping beat!");
		break;
	}

	return bestClassification;
}

void
BeatClassifier::ProgressToNextBar()
{
	LOG("Switching to next bar in classification");

	currentBarStart_ = nextBarStart_;
	currentTimeSignature_ = currentBarStart_.meter();
	nextBarStart_ = tempoMap_.GetScorePositionAt(currentBarStart_.BeginningOfNextBar(), currentBarStart_.time());
}

void
BeatClassifier::AddEstimatorsForPattern(Data::BeatPattern const & pattern)
{
	estimators_.insert(std::make_pair(pattern.meter, pattern));
}

} // namespace ScoreFollower
} // namespace cf