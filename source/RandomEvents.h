/* Copyright (c) 2012-2015 Fabian Schuiki */
#pragma once

namespace OT {

class Game;

/** Handles random events (VIP visit, etc.) that affect rating and messages. */
class RandomEvents
{
public:
	explicit RandomEvents(Game * game);
	void advance(double dt);

	// Called by Game when the player responds to a terrorist event prompt.
	void onTerroristPayRansom();
	void onTerroristDispatchSecurity();

private:
	Game * game;

	enum VipState { VipNone, VipScheduled, VipArrived, VipLeft };
	VipState vipState;
	double vipArrivalTime;
	double vipDepartureTime;
	int lastVipScheduleYear;

	void onVipArrived();
	void onVipLeft();
	bool computeVipSatisfaction() const;

	// One-shot / periodic random events.
	bool goldDiscovered;
	int lastSantaYear;

	enum TerroristState { TerroristNone, TerroristAwaitChoice, TerroristSearching, TerroristResolved, TerroristExploded };
	TerroristState terroristState;
	double terroristDeadlineTime;
	double terroristSearchEndTime;

	void maybeTriggerGoldDiscovery();
	void maybeTriggerSanta();
	void maybeTriggerTerroristEvent();
	void resolveTerroristSearch();
};

}
