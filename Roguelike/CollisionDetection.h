#pragma once
#include "Treasure.h"
#include "Player.h";
#include "modelclass.h";

static class CollisionDetection
{
public:
	static bool SAT(Player*, Treasure*);
};