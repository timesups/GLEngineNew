#pragma once
#include "Entity.hpp"

class EntityManager
{
public:
	~EntityManager() {}
	static EntityManager* Get() 
	{
		if (!instance)
			instance = new EntityManager();
		return instance;
	}
private:
	static EntityManager* instance;
	EntityManager();
};

EntityManager* EntityManager::instance = nullptr;