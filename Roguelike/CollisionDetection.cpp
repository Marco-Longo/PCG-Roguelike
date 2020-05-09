#include "pch.h"
#include "CollisionDetection.h"

bool CollisionDetection::SAT(Player* player, Treasure* rect)
{
	float minX1, maxX1, minY1, maxY1, minZ1, maxZ1;
	float minX2, maxX2, minY2, maxY2, minZ2, maxZ2;

	minX1 = player->GetPosition().x - (player->GetWidth() / 2.0f);
	maxX1 = minX1 + player->GetWidth();
	minY1 = player->GetPosition().y - (player->GetHeight() / 2.0f);
	maxY1 = minY1 + player->GetHeight();

	minX2 = rect->GetPosition().x - (rect->GetWidth() / 2.0f);
	maxX2 = minX2 + rect->GetWidth();
	minY2 = rect->GetPosition().y - (rect->GetHeight() / 2.0f);
	maxY2 = minY2 + rect->GetHeight();

	return  (minX1 <= maxX2 && maxX1 >= minX2) &&
			(minY1 <= maxY2 && maxY1 >= minY2);
}