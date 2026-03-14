/* Copyright (c) 2012-2015 Fabian Schuiki */
#include <cstdlib>
#include <algorithm>
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
	lastVipScheduleYear(0),
	goldDiscovered(false),
	lastSantaYear(0),
	terroristState(TerroristNone),
	terroristDeadlineTime(0),
	terroristSearchEndTime(0)
{
}

void RandomEvents::advance(double dt)
{
	Time & t = game->time;

	// Fire-and-forget events that don't need state machines.
	maybeTriggerGoldDiscovery();
	maybeTriggerSanta();
	maybeTriggerTerroristEvent();

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

void RandomEvents::maybeTriggerGoldDiscovery()
{
	// One-time: if the player has built any underground floors, occasionally discover gold.
	if (goldDiscovered) return;
	if (game->floorItems.empty()) return;
	if (game->floorItems.begin()->first >= 0) return; // no floors below ground

	// Rare chance per game-day once underground exists.
	if (!game->time.checkTick(1.0)) return;
	// ~1/120 per day => on average once every ~40 in-game months (rare).
	if (Math::randi(0, 119) != 0) return;

	goldDiscovered = true;
	const int payout = 250000;
	game->timeWindow.showMessage("Workers discovered gold underground!");
	game->transferFunds(payout, "Gold discovery");
	game->playOnce("simtower/cash");
	LOG(IMPORTANT, "Gold discovered event: payout=%i", payout);
}

void RandomEvents::maybeTriggerSanta()
{
	// End of Q4 each year. With Time's calendar: year advances every 12 absolute days.
	// Trigger right at the start of each year (i.e., after passing a multiple of 12 days).
	if (!game->time.checkTick(12.0)) return;
	if (game->time.year <= 1) return; // skip at game start
	if (lastSantaYear == game->time.year) return;
	lastSantaYear = game->time.year;

	game->timeWindow.showMessage("Santa flew over the tower!");
	game->playOnce("simtower/bells");
	LOG(IMPORTANT, "Santa event triggered for year %i", game->time.year);
}

void RandomEvents::maybeTriggerTerroristEvent()
{
	// Terrorist/bomb event: rare, requires a running simulation.
	if (terroristState == TerroristResolved || terroristState == TerroristExploded) {
		terroristState = TerroristNone;
	}
	if (terroristState != TerroristNone && terroristState != TerroristSearching && terroristState != TerroristAwaitChoice)
		return;

	// If searching, resolve when timer elapses or deadline passes.
	if (terroristState == TerroristSearching) {
		if (game->time.check(terroristSearchEndTime) || game->time.check(terroristDeadlineTime)) {
			resolveTerroristSearch();
		}
		return;
	}

	// If awaiting choice, just wait for Game callbacks.
	if (terroristState == TerroristAwaitChoice) return;

	// Only attempt to trigger once per day.
	if (!game->time.checkTick(1.0)) return;
	if (game->rating < 1) return; // at least 2 stars feels more appropriate, but keep available earlier
	if (game->population < 500) return;

	// Very rare chance per day.
	if (Math::randi(0, 199) != 0) return; // 1/200 per day

	terroristState = TerroristAwaitChoice;
	terroristDeadlineTime = game->time.absolute + 0.25; // ~6 in-game hours to resolve
	game->timeWindow.showMessage("Terrorist threat reported! Decide quickly.");
	game->playOnce("simtower/alerts/terrorist");
	LOG(IMPORTANT, "Terrorist event triggered; deadline absolute=%.2f", terroristDeadlineTime);

	// Ask the player.
	// This pauses the simulation until a choice is made.
	game->openTerroristEventDialog();
}

void RandomEvents::onTerroristPayRansom()
{
	if (terroristState != TerroristAwaitChoice) return;
	terroristState = TerroristResolved;
	const int cost = 150000;
	game->transferFunds(-cost, "Paid ransom");
	game->timeWindow.showMessage("Ransom paid. Threat averted.");
	LOG(IMPORTANT, "Terrorist resolved by ransom; cost=%i", cost);
}

void RandomEvents::onTerroristDispatchSecurity()
{
	if (terroristState != TerroristAwaitChoice) return;

	terroristState = TerroristSearching;
	terroristSearchEndTime = game->time.absolute + 0.15; // ~4–5 in-game hours
	game->timeWindow.showMessage("Security is searching the tower...");
	LOG(IMPORTANT, "Terrorist: security search started; ends absolute=%.2f", terroristSearchEndTime);
}

void RandomEvents::resolveTerroristSearch()
{
	if (terroristState != TerroristSearching) return;

	bool hasSecurity = (game->itemsByType.count("security") != 0 && !game->itemsByType["security"].empty());
	const bool beforeDeadline = (game->time.absolute < terroristDeadlineTime);

	bool resolved = false;
	if (hasSecurity && beforeDeadline) {
		// 75% chance to resolve successfully if security exists and is timely.
		resolved = (Math::randi(0, 3) != 0);
	}

	if (resolved) {
		terroristState = TerroristResolved;
		game->timeWindow.showMessage("Security contained the threat. Well done.");
		game->playOnce("simtower/applause");
		LOG(IMPORTANT, "Terrorist resolved by security");
	} else {
		terroristState = TerroristExploded;
		game->timeWindow.showMessage("Explosion! The tower's reputation suffered.");
		game->playOnce("simtower/alerts/terrorist");
		game->setRating(std::max(0, game->rating - 1));
		LOG(IMPORTANT, "Terrorist exploded; rating decreased to %i", game->rating);
	}
}
