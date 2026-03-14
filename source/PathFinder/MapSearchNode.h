#pragma once
#include <stlastar.h>
#include "GameMap.h"
#include "MapNode.h"


namespace OT { 
	namespace Item { class Item; }
}

class MapSearchNode {
public:
	const OT::MapNode *mapNode;
	OT::Item::Item *parent_item;
	int numStairs;
	int numEscalators;
	int numElevators;
	float g;
	float h;
	OT::MapNode::Point start_point;
	OT::MapNode::Point end_point;
	bool serviceRoute;

	MapSearchNode();
	explicit MapSearchNode(const OT::MapNode *mp);

	float GetCost(MapSearchNode &successor);
	float GoalDistanceEstimate(MapSearchNode &nodeGoal);
	bool GetSuccessors(AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node);
	bool IsGoal(MapSearchNode &nodeGoal);
	bool IsSameState(MapSearchNode &rhs);

	OT::Item::Item * getItemOnRoute(const MapSearchNode *successor);
	bool canTransfer(const MapSearchNode *start, const OT::MapNode *dest, const OT::MapNode::Direction dir) const;
	void createNode(MapSearchNode &n, const OT::MapNode *node);

private:
	static const int MAX_WALKING_DIST = 80;
	static const int WALKING_COST	  = 1;
	static const int FLOOR_COST		  = 5;
	static const int ESCALATOR_COST	  = 10;
	static const int STAIRS_COST	  = 30;
	/** Balanced so elevators are viable for multi-floor trips; express preferred over standard. */
	static const int ELEVATOR_COST	  = 45;
	static const int EXPRESS_COST	  = 12;
	static const int INHIBITORY_COST  = 10000;
};
