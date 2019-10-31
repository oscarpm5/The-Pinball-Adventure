#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ball = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	ball = App->textures->Load("pinball/ball.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	map = App->textures->Load("pinball/map.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	int map[176] = {
		527, 669,
		527, 247,
		517, 200,
		499, 154,
		483, 119,
		462, 92,
		432, 64,
		393, 42,
		360, 26,
		315, 15,
		287, 8,
		235, 8,
		207, 15,
		163, 26,
		141, 37,
		107, 52,
		85, 65,
		29, 120,
		18, 143,
		7, 164,
		7, 193,
		23, 226,
		23, 241,
		18, 258,
		13, 260,
		13, 325,
		18, 326,
		19, 336,
		24, 337,
		35, 360,
		45, 365,
		68, 376,
		80, 388,
		91, 405,
		91, 417,
		79, 427,
		61, 439,
		41, 451,
		41, 683,
		57, 684,
		130, 712,
		180, 729,
		207, 757,
		208, 842,
		-93, 842,
		-93, -62,
		770, -60,
		771, 835,
		308, 832,
		309, 756,
		337, 728,
		460, 685,
		482, 677,
		482, 445,
		468, 445,
		427, 416,
		426, 400,
		477, 350,
		482, 349,
		482, 227,
		467, 192,
		440, 136,
		394, 92,
		336, 65,
		288, 53,
		261, 48,
		208, 58,
		162, 74,
		87, 135,
		68, 166,
		57, 187,
		49, 187,
		49, 176,
		71, 142,
		104, 109,
		135, 80,
		162, 65,
		185, 54,
		210, 47,
		238, 41,
		287, 42,
		313, 48,
		348, 59,
		392, 80,
		448, 135,
		484, 203,
		488, 225,
		488, 669
	};

	int left_platform[14] = {
		77, 466,
		77, 600,
		141, 642,
		160, 642,
		160, 625,
		93, 580,
		93, 478
	};

	int right_platform[14] = {
		360, 642,
		360, 628,
		427, 577,
		427, 476,
		441, 464,
		441, 599,
		378, 642
	};

	int top_platform[42] = {
		118, 210,
		218, 209,
		218, 214,
		225, 215,
		241, 200,
		285, 199,
		301, 214,
		308, 214,
		308, 211,
		408, 211,
		416, 203,
		416, 179,
		401, 152,
		367, 116,
		325, 100,
		265, 90,
		207, 99,
		167, 110,
		133, 143,
		110, 180,
		109, 202
	};
	//ricks.add(App->physics->CreateChain(0, 0, rick_head, 176));
	App->physics->CreateChain(0, 0, map, 176);
	App->physics->CreateChain(0, 0, left_platform, 14);
	App->physics->CreateChain(0, 0, right_platform, 14);
	App->physics->CreateChain(0, 0, top_platform, 42);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 768;
	rect.h = 768;
	
	App->renderer->Blit(map, 0, 0, &rect);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 10));
		circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
	
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		
		App->renderer->Blit(ball, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}