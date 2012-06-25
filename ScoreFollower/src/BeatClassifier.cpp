#include "BeatClassifier.h"

#include "globals.h"

#include <boost/math/distributions/normal.hpp>

namespace cf {
namespace ScoreFollower {

BeatClassifier::Result
BeatClassifier::ClassifyBeat(double quartersSincePreviousBeat)
{
	double const eightDeviation = 1.0 / 16;
	double const quarterDeviation = 1.0 / 6;

	int bestPos = -1;
	double bestProb = -1.0;
	double bestProbNormalized = -1.0;

	// TODO optimize out unnecessary estimates

	// First check if this is an eight
	bestPos = 1;
	bestProb = ProbabilityAt(quartersSincePreviousBeat, bestPos, eightDeviation, bestProbNormalized);

	// Now check qarters
	for (int pos = 2; pos < 128; pos += 2) {
		double probNormalized = -1.0;
		double prob = ProbabilityAt(quartersSincePreviousBeat, pos, quarterDeviation, probNormalized);

		if (prob > bestProb) {
			bestPos = pos;
			bestProb = prob;
			bestProbNormalized = probNormalized;
		} else if (bestProb > 0.001) {
			return Result(bestPos, bestProbNormalized);
		}
	}

	// TODO stop the whole thing now!
	return Result();
}

double
BeatClassifier::ProbabilityAt(double position, int meanAsEights, double deviation, double & normalizedProb)
{
	double mean = static_cast<double>(meanAsEights) / 2;
	boost::math::normal dist(mean, deviation);
	double prob = boost::math::pdf(dist, position);
	normalizedProb = prob / boost::math::pdf(dist, mean);
	return prob;
}


} // namespace ScoreFollower
} // namespace cf