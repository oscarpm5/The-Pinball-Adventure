#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Animations.h"

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
	void BlitScore();
	void GetTopHighScores(int& first, int& second, int& third);
	void ModuleSceneIntro::BubbleSort(int arr[], int n);
	void ModuleSceneIntro::Swap(int* xp, int* yp);
	bool ModuleSceneIntro::Reset();//resets lives and score in between games

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> left_flippers;
	p2List<PhysBody*> right_flippers;
	p2List<PhysBody*> kickers;
	p2List<PhysBody*> frogs;
	p2List<PhysBody*> red_sensors;
	p2List<PhysBody*> chains;

	p2List<Animations*> animations;

	PhysBody* sensor;
	PhysBody* leftbumper;
	PhysBody* rightbumper;
	bool sensed;

	SDL_Texture* ball;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* map;
	SDL_Texture* left_flipper;
	SDL_Texture* right_flipper;
	SDL_Texture* right_bumper;
	SDL_Texture* left_bumper;
	SDL_Texture* kicker;
	SDL_Texture* frog;
	SDL_Texture* red_sensor;
	uint bonus_fx;
	uint flipper_fx;
	uint start_fx;
	uint bounce_fx;
	uint coins_fx;
	p2Point<int> ray;

	p2List<int> highscore_list;
	bool ray_on;
	bool alreadycreated = false;
	bool alreadyfinished = false;
	bool isleftbumping = false;
	bool isrightbumping = false;
	int lives = 3;
	int score = 0;
	int highscore = 0;
	char score_char[10];
	char lives_char[10];
	char highscore_char[10];

	char top1score_char[10];
	char top2score_char[10];
	char top3score_char[10];


};
