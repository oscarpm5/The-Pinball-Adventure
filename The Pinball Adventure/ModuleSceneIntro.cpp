#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"

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
	left_flipper = App->textures->Load("pinball/left_flipper.png");
	right_flipper = App->textures->Load("pinball/rigth_flipper.png");
	frog = App->textures->Load("pinball/froganim.png");
	red_sensor = App->textures->Load("pinball/red_sensor.png");
	left_bumper = App->textures->Load("pinball/left_bumper.png");
	right_bumper = App->textures->Load("pinball/right_bumper.png");

	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	flipper_fx = App->audio->LoadFx("pinball/flipper.wav");
	start_fx = App->audio->LoadFx("pinball/start.wav");
	bounce_fx = App->audio->LoadFx("pinball/bounce.wav");
	coins_fx = App->audio->LoadFx("pinball/coins.wav");

	App->audio->PlayFx(start_fx, 0);
	App->audio->PlayMusic("pinball/music.ogg", 3.0f);
	kicker = App->textures->Load("pinball/kicker.png");

	App->fonts->Load("pinball/font.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.:;[]0123456789           ", 3);
	sensor = App->physics->CreateRectangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 10, true, 0, true);

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

	int left_platform[12] = {
			150, 616,
			92, 577,
			91, 472,
			76, 463,
			76, 597,
			140, 640
	};

	int right_platform[12] = {
		373, 644,
		441, 601,
		441, 466,
		431, 475,
		431, 576,
		365, 620
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

	int left_bumper[8] = {
		119, 492,
		118, 557,
		165, 596,
		164, 572
	};

	int right_bumper[8] = {
	400, 491,
	354, 576,
	355, 594,
	399, 556
	};

	chains.add(App->physics->CreateChain(0, 0, map, 176, 0.3f));
	chains.add(App->physics->CreateChain(0, 0, left_platform, 12, 0.3f));
	chains.add(App->physics->CreateChain(0, 0, right_platform, 12, 0.3f));
	chains.add(App->physics->CreateChain(0, 0, top_platform, 42, 0.3f));
	chains.add(App->physics->CreateChain(0, 0, left_bumper, 8, 0.3f));
	chains.add(App->physics->CreateChain(0, 0, right_bumper, 8, 0.3f));

	frogs.add(App->physics->CreateCircle(80, 318, 20, false, 0.9f));//left center frog
	frogs.add(App->physics->CreateCircle(205, 292, 20, false, 0.9f));//left upper frog
	frogs.add(App->physics->CreateCircle(150, 370, 20, false, 0.9f));//left lower frog

	frogs.add(App->physics->CreateCircle(440, 318, 20, false, 0.9f));//right center frog
	frogs.add(App->physics->CreateCircle(320, 292, 20, false, 0.9f));//right upper frog
	frogs.add(App->physics->CreateCircle(375, 370, 20, false, 0.9f));//right lower frog

	circles.add(App->physics->CreateCircle(508, 502, 10, true, -1, true)); //first ball
	circles.getLast()->data->listener = this;

	red_sensors.add(App->physics->CreateRectangle(58, 459, 35, 15, true, 0.6, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(460, 459, 35, 15, true, -0.6, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(140, 225, 35, 15, true, 0, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(180, 225, 35, 15, true, 0, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(340, 225, 35, 15, true, 0, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(380, 225, 35, 15, true, 0, false, 1.75f));

	leftbumper = App->physics->CreateRectangle(144, 535, 80, 10, true, 1.06f, true);
	rightbumper = App->physics->CreateRectangle(376, 535, 80, 10, true, 2.06f, true);



	//Inicializes the highscore table with 3 0's to avoid errors
	highscore_list.add(0);
	highscore_list.add(0);
	highscore_list.add(0);

	//inicialize animations

	Animations* froganim = new Animations;
	froganim->AddFrame(120, { 0,0,72,38 }, { 0,0 });
	froganim->AddFrame(120, { 0,39,72,38 }, { 0,0 });
	froganim->AddFrame(120, { 0,78,72,38 }, { 0,0 });
	froganim->animationname = "frog";
	froganim->animationfinished = false;
	froganim->animationloop = 1;

	animations.add(froganim);

	Animations* leftbumperanim = new Animations;
	leftbumperanim->AddFrame(1, { 0,0,51,118 }, { 0,0 });
	leftbumperanim->AddFrame(60, { 52,0,51,118 }, { 0,0 });
	leftbumperanim->animationname = "left_bumper";
	leftbumperanim->animationfinished = false;
	leftbumperanim->animationloop = 0;

	animations.add(leftbumperanim);
	Animations* rightbumperanim = new Animations;
	rightbumperanim->AddFrame(1, { 0,0,51,118 }, { 0,0 });
	rightbumperanim->AddFrame(60, { 52,0,51,118 }, { 0,0 });
	rightbumperanim->animationname = "right_bumper";
	rightbumperanim->animationfinished = false;
	rightbumperanim->animationloop = 0;

	animations.add(rightbumperanim);
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	//Unload Circles
	p2List_item<PhysBody*>* item = circles.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	circles.clear();

	//Unload Frogs
	item = frogs.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	frogs.clear();

	//Unload RedSensors
	item = red_sensors.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	red_sensors.clear();

	//Unload Kickers
	item = kickers.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	kickers.clear();

	//unload Flippers
	item = right_flippers.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	right_flippers.clear();

	item = left_flippers.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	left_flippers.clear();

	//Unload Ricks
	item = ricks.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	ricks.clear();

	//Unload Boxes
	item = boxes.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	boxes.clear();

	//Unload Chains
	item = chains.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	chains.clear();

	//Unload other elements
	if (sensor != NULL)
		App->physics->world->DestroyBody(sensor->body);
	if (leftbumper != NULL)
		App->physics->world->DestroyBody(leftbumper->body);
	if (rightbumper != NULL)
		App->physics->world->DestroyBody(rightbumper->body);




	highscore_list.clear();


	//Unload textures
	App->textures->Unload(ball);
	App->textures->Unload(box);
	App->textures->Unload(rick);
	App->textures->Unload(map);
	App->textures->Unload(left_flipper);
	App->textures->Unload(right_flipper);
	App->textures->Unload(frog);
	App->textures->Unload(red_sensor);
	App->textures->Unload(kicker);

	//Unload text
	App->fonts->UnLoad(0);

	return true;
}
bool ModuleSceneIntro::Reset()
{
	App->audio->PlayFx(3, 0);
	circles.add(App->physics->CreateCircle(508, 502, 10, true, -1, true));

	p2List_item<PhysBody*>* item = red_sensors.getFirst();
	while (item != NULL)
	{
		App->physics->world->DestroyBody(item->data->body);
		item = item->next;
	}
	red_sensors.clear();

	red_sensors.add(App->physics->CreateRectangle(58, 459, 35, 15, true, 0.6, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(460, 459, 35, 15, true, -0.6, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(140, 225, 35, 15, true, 0, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(180, 225, 35, 15, true, 0, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(340, 225, 35, 15, true, 0, false, 1.75f));
	red_sensors.add(App->physics->CreateRectangle(380, 225, 35, 15, true, 0, false, 1.75f));
	lives = 3;
	score = 0;
	alreadyfinished = false;

	//Resets all the animations
	p2List_item<Animations*>* animitem = animations.getFirst();

	while (animitem != NULL)
	{
		animitem->data->ResetAnimation();
		animitem = animitem->next;
	}
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

	if (lives != 0) {
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			ray_on = !ray_on;
			ray.x = App->input->GetMouseX();
			ray.y = App->input->GetMouseY();
		}

		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 10, true, -1, true));
			circles.getLast()->data->listener = this;
		}

		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50, true));
		}


		// Flippers' Motors Logic -------------------------------------

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
			App->audio->PlayFx(App->scene_intro->flipper_fx, 0);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{

			App->physics->flipper_joint_left->EnableMotor(true);
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
		{
			App->physics->flipper_joint_left->EnableMotor(false);
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			App->physics->flipper_joint_right->EnableMotor(true);
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
		{
			App->physics->flipper_joint_right->EnableMotor(false);
		}

		//kickers Motors Logic-----------------------------------------

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			App->physics->kicker_joint->SetMotorSpeed(2);
		}
		else
		{
			App->physics->kicker_joint->SetMotorSpeed(-15);
		}
	}


	// Prepare for raycast ------------------------------------------------------

	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = frogs.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);

		FrameInfo* frame = animations.getFirst()->data->StepAnimation();

		App->renderer->Blit(frog, x - 15, y, &frame->animationRect, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = circles.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);

		App->renderer->Blit(ball, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = left_flippers.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(left_flipper, x - 8, y - 6, NULL, 1.0f, c->data->GetRotation());
		if (ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = right_flippers.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(right_flipper, x - 6, y - 4, NULL, 1.0f, c->data->GetRotation());
		if (ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = kickers.getFirst();
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);

		App->renderer->Blit(kicker, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = red_sensors.getFirst();
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);

		App->renderer->Blit(red_sensor, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}


	//Bumper animations---------------------------
	//left
	FrameInfo* frame1anim = animations.getFirst()->next->data->currentanimframe->data;
	if (isleftbumping == true)
	{
		frame1anim = animations.getFirst()->next->data->StepAnimation();
		if (animations.getFirst()->next->data->GetAnimationFinish())
		{
			isleftbumping = false;
			animations.getFirst()->next->data->ResetAnimation();
		}
	}
	App->renderer->Blit(left_bumper, 117, 475, &frame1anim->animationRect);

	//right
	FrameInfo* frame2anim = animations.getFirst()->next->next->data->currentanimframe->data;
	if (isrightbumping == true)
	{
		frame2anim = animations.getFirst()->next->next->data->StepAnimation();
		if (animations.getFirst()->next->next->data->GetAnimationFinish())
		{
			isrightbumping = false;
			animations.getFirst()->next->next->data->ResetAnimation();
		}
	}
	App->renderer->Blit(right_bumper, 352, 475, &frame2anim->animationRect);


	// ray -----------------
	if (ray_on == true)
	{
		fVector destination(mouse.x - ray.x, mouse.y - ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if (normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	BlitScore();
	if ((lives == 2 || lives == 1) && alreadycreated == false) {

		circles.add(App->physics->CreateCircle(508, 502, 10, true, -1, true));
		circles.getLast()->data->listener = this;
		alreadycreated = true;
	}
	else if (lives == 0) {

		if (alreadyfinished == false)
			highscore_list.add(score);

		// Sets flippers to original pos ----------------
		App->physics->flipper_joint_left->EnableMotor(false);
		App->physics->flipper_joint_right->EnableMotor(false);

		int first;
		int second;
		int third;
		GetTopHighScores(first, second, third);
		highscore = first;

		sprintf_s(top1score_char, 10, "%d", first);
		sprintf_s(top2score_char, 10, "%d", second);
		sprintf_s(top3score_char, 10, "%d", third);


		SDL_Rect black_square;
		black_square.w = SCREEN_WIDTH;
		black_square.h = SCREEN_HEIGHT;
		black_square.x = (SCREEN_WIDTH * 0.5) - (black_square.w * 0.5);
		black_square.y = (SCREEN_HEIGHT * 0.5) - (black_square.h * 0.5);

		App->renderer->DrawQuad(black_square, 0, 0, 0, 230, true, false);

		App->fonts->BlitText(230, 100, 0, "GAME OVER", 6.0f);
		App->fonts->BlitText(280, 600, 0, "Start Again", 2.0f);
		App->fonts->BlitText(430, 602, 0, "[Enter]", 1.5f);
		App->fonts->BlitText(280, 650, 0, "Exit", 2.0f);
		App->fonts->BlitText(422, 652, 0, "[Escape]", 1.5f);

		//your score
		App->fonts->BlitText(300, 175, 0, "YOUR SCORE:", 2.0f);
		App->fonts->BlitText(445, 175, 0, score_char, 2.0f);

		//highscores
		App->fonts->BlitText(285, 280, 0, "HIGSCORES", 3.5f);

		//1
		App->fonts->BlitText(250, 350, 0, "1. ", 2.5f);
		App->fonts->BlitText(280, 350, 0, top1score_char, 2.5f);

		//2
		App->fonts->BlitText(250, 410, 0, "2. ", 2.5f);
		App->fonts->BlitText(280, 410, 0, top2score_char, 2.5f);

		//3
		App->fonts->BlitText(250, 470, 0, "3. ", 2.5f);
		App->fonts->BlitText(280, 470, 0, top3score_char, 2.5f);


		alreadyfinished = true;
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {

			Reset();
		}



	}


	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	if (bodyB == sensor && lives > 0) {
		alreadycreated = false;
		lives--;
	}
	if (bodyB == rightbumper || bodyB == leftbumper)
	{
		bodyA->body->ApplyLinearImpulse({ 0,-0.015f }, bodyA->body->GetWorldCenter(), true);
	}
}

void ModuleSceneIntro::BlitScore() {

	App->fonts->BlitText(555, 260, 0, "Pause", 1.8f);
	App->fonts->BlitText(640, 260, 0, "SFX", 1.8f);
	App->fonts->BlitText(695, 260, 0, "Music", 1.8f);

	App->fonts->BlitText(550, 298, 0, "Lives", 1.8f);
	sprintf_s(lives_char, 10, "%d", lives);
	App->fonts->BlitText(550, 315, 0, lives_char, 1.8f);

	App->fonts->BlitText(550, 339, 0, "Score", 1.8f);
	//App->fonts->BlitText(550, 360, 0, );
	sprintf_s(score_char, 10, "%d", score);
	App->fonts->BlitText(550, 360, 0, score_char, 1.8f);

	App->fonts->BlitText(630, 339, 0, "Highscore", 1.8f);
	sprintf_s(highscore_char, 10, "%d", highscore);
	App->fonts->BlitText(630, 360, 0, highscore_char, 1.8f);


}

void ModuleSceneIntro::GetTopHighScores(int& first, int& second, int& third)
{
	int elements = highscore_list.count();

	int* int_list = new int[elements];

	p2List_item<int>* item = highscore_list.getFirst();


	//takes all the scores and puts them into an array
	for (int i = 0; i < elements; i++)
	{
		int_list[i] = item->data;
		item = item->next;
	}

	BubbleSort(int_list, elements);

	first = int_list[0];
	second = int_list[1];
	third = int_list[2];

}

void ModuleSceneIntro::BubbleSort(int arr[], int n)
{

	int i, j;
	for (i = 0; i < n - 1; i++)

		// Last i elements are already in place  
		for (j = 0; j < n - i - 1; j++)
			if (arr[j] < arr[j + 1])
				Swap(&arr[j], &arr[j + 1]);
}

void ModuleSceneIntro::Swap(int* xp, int* yp)
{
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

