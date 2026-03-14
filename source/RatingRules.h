/* Copyright (c) 2012-2015 Fabian Schuiki */
#pragma once

/** Single source of truth for star rating thresholds (SimTower-style).
 *  Rating index: 0 = 1 star, 1 = 2 stars, 2 = 3 stars, 3 = 4 stars, 4 = 5 stars, 5 = Tower. */

namespace OT {

struct RatingRules {
	/** Minimum population to reach 2 stars (from 1 star). */
	static const int kRating2Population = 300;
	/** Minimum number of Security offices required to reach 2 stars. */
	static const int kRating2SecurityCount = 1;

	/** Minimum population to reach 3 stars (from 2 stars). */
	static const int kRating3Population = 1000;
	/** Minimum number of Security offices required to reach 3 stars. */
	static const int kRating3SecurityCount = 1;

	/** Minimum population to reach 4 stars (from 3 stars). Also requires 2+ hotel suites, recycling, medical, favorable VIP. */
	static const int kRating4Population = 5000;
	static const int kRating4HotelSuites = 2;

	/** Minimum population to reach 5 stars (from 4 stars). */
	static const int kRating5Population = 10000;

	/** Minimum population for Tower (from 5 stars). */
	static const int kTowerPopulation = 15000;

	// Building limits (SimTower)
	static const int kMaxFloorsAboveGround = 100;
	static const int kMaxFloorsBelowGround = 9;
	static const int kMaxElevatorShafts = 24;
	static const int kMaxCarsPerElevator = 8;
};

}
