#pragma once
#include "Boundary.h"
#include "Player.h";
#include "modelclass.h";

static class CollisionDetection
{
public:
	static bool SAT(Player*, Boundary*);
};