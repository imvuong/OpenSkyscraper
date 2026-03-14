/* Copyright (c) 2012-2015 Fabian Schuiki */
#include <cstdlib>
#include "Game.h"
#include "Logger.h"
#include "Math/Rand.h"
#include "RandomEvents.h"
#include "RatingRules.h"
#include "Time.h"

using namespace OT;

RandomEvents::RandomEvents(Game * game)
:	game(game),
	vipState(VipNone),
	vipArrivalTime(0),
	vipDepartureTime(0),
	lastVipScheduleYear(0)
{
}

void RandomEvents::advance(double dt)
{
	Time & t = game->time;

	// VIP visit state machine
	switch (vipState) {
		case VipNone: {
			// At start of a new year, with some probability schedule a VIP visit (only if 3 stars and not already favorable)
			if (game->rating >= 2 && !game->favorableVipVisit && t.year > 0) {
				if (t.year > lastVipScheduleYear) {
					lastVipScheduleYear = t.year;
					// ~25% chance per year at 3 stars to schedule a VIP
					if (Math::randi(0, 3) == 0) {
						vipArrivalTime = t.absolute + 2.0; // arrive in 2 game-days
						vipState = VipScheduled;
						game->timeWindow.showMessage("A VIP will visit in 2 days!");
						LOG(IMPORTANT, "VIP visit scheduled for absolute time %.2f", vipArrivalTime);
					}
				}
			}
		} break;
		case VipScheduled: {
			if (t.check(vipArrivalTime)) {
				onVipArrived();
			}
		} break;
		case VipArrived: {
			if (t.check(vipDepartureTime)) {
				onVipLeft();
			}
		} break;
		case VipLeft:
			/* not used; we set VipNone in onVipLeft */
			break;
	}
}

void RandomEvents::onVipArrived()
{
	vipState = VipArrived;
	vipDepartureTime = game->time.absolute + 0.15; // stay ~4–5 in-game hours
	game->timeWindow.showMessage("The VIP has arrived!");
	game->playOnce("simtower/alerts/vip");
	LOG(IMPORTANT, "VIP arrived, departs at absolute %.2f", vipDepartureTime);
}

void RandomEvents::onVipLeft()
{
	bool satisfied = computeVipSatisfaction();
	vipState = VipNone; // allow scheduling again next year
	if (satisfied) {
		game->favorableVipVisit = true;
		game->timeWindow.showMessage("The VIP enjoyed their visit! Favorable rating received.");
		game->playOnce("simtower/applause");
		game->ratingMayIncrease();
		LOG(IMPORTANT, "VIP left satisfied; favorableVipVisit set");
	} else {
		game->timeWindow.showMessage("The VIP left unimpressed.");
		LOG(IMPORTANT, "VIP left unsatisfied");
	}
}

bool RandomEvents::computeVipSatisfaction() const
{
	int hotelSuiteCount = 0;
	if (game->itemsByType.count("suite") != 0)
		hotelSuiteCount = (int)game->itemsByType["suite"].size();
	bool hasRestaurant = (game->itemsByType.count("restaurant") != 0 && !game->itemsByType["restaurant"].empty());

	// Satisfied if: 2+ hotel suites and at least one restaurant, with 90% probability
	if (hotelSuiteCount >= RatingRules::kRating4HotelSuites && hasRestaurant)
		return (Math::randi(0, 9) != 0); // 90% favorable
	return false;
}
