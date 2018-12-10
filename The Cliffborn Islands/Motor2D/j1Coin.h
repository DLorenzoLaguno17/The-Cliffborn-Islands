#ifndef __j1COIN_H__
#define __j1COIN_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "j1Collisions.h"

class j1Coin : public j1Entity
{
public:
	
	j1Coin(int x, int y, ENTITY_TYPES type);
	
	virtual ~j1Coin();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	Animation idle;
};

#endif //__j1COIN_H__