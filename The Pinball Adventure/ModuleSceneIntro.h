#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> left_flippers;
	p2List<PhysBody*> right_flippers;
	p2List<PhysBody*> kickers;

	PhysBody* sensor;
	bool sensed;

	SDL_Texture* ball;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* map;
	SDL_Texture* left_flipper;
	SDL_Texture* right_flipper;
	SDL_Texture* kicker;
	uint bonus_fx;
	uint flipper_fx;
	uint start_fx;
	p2Point<int> ray;
	bool ray_on;
};
