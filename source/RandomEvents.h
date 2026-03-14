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

private:
	Game * game;

	enum VipState { VipNone, VipScheduled, VipArrived, VipLeft };
	VipState vipState;
	double vipArrivalTime;
	double vipDepartureTime;
	int lastVipScheduleYear;

	void tryScheduleVipVisit();
	void onVipArrived();
	void onVipLeft();
	bool computeVipSatisfaction() const;
};

}
