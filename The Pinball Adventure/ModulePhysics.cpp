#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	// LEFT FLIPPER
	CreateFlipper(163, 635, 198, 640, 95, 20, 0); // Left == 0 
	CreateFlipper(355, 637, 316, 640, 95, 20, 1); // Right == 1
	CreateKicker(507, 660, 507, 660, 30, 19);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateFlipper(int x, int y, int x1, int y1, int width, int height, int left_right)
{
	// Creation of the static ball (needed for joint)
	b2BodyDef ball;
	ball.type = b2_staticBody;
	ball.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* ball_joint = world->CreateBody(&ball);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(5) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	ball_joint->CreateFixture(&fixture);

	// Creation of the flipper body

	b2BodyDef flipper;
	flipper.type = b2_dynamicBody;
	flipper.position.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	flipper.allowSleep = false;

	b2Body* flipper_body = world->CreateBody(&flipper);
	b2PolygonShape flipper_1;
	flipper_1.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture1;
	fixture1.shape = &flipper_1;
	fixture1.density = 1.0f;

	flipper_body->CreateFixture(&fixture1);

	PhysBody* pbody = new PhysBody();
	pbody->body = flipper_body;
	flipper_body->SetUserData(pbody);
	pbody->width = 90 * 0.5f;
	pbody->height = 10 * 0.5f;

	// Creation of the revolution joint 

	jointDef1.Initialize(ball_joint, flipper_body, ball_joint->GetWorldCenter());
	jointDef1.enableMotor = false;

	if (left_right == 0)
		jointDef1.motorSpeed = -revolute_joint_speed;
	if (left_right == 1)
		jointDef1.motorSpeed = revolute_joint_speed;

	jointDef1.maxMotorTorque = 1000;
	jointDef1.enableLimit = true;
	jointDef1.lowerAngle = -0.523599;
	jointDef1.upperAngle = 0.523599;

	if (left_right == 0) {
		App->scene_intro->left_flippers.add(pbody);
		flipper_joint_left = (b2RevoluteJoint*)world->CreateJoint(&jointDef1);
	}

	if (left_right == 1) {
		App->scene_intro->right_flippers.add(pbody);
		flipper_joint_right = (b2RevoluteJoint*)world->CreateJoint(&jointDef1);
	}

	return pbody;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, bool dynamic, float restitution, bool alwaysactive)
{
	b2BodyDef body;
	if (dynamic)
		body.type = b2_dynamicBody;
	else
		body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	if (alwaysactive == true)
	{
		body.allowSleep = false;
		body.active = true;
		body.awake = true;
	}

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.01f;
	if (restitution != -1.0f)
	{
		fixture.restitution = restitution;//restitution value can range between 0 and 1 in normal conditions. It determines the bounciness
	}
	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, bool dynamic, float angle, bool sensor, float restitution)
{
	b2BodyDef body;
	body.angle = angle;
	if (!dynamic)
		body.type = b2_dynamicBody;
	else
		body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);


	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = sensor;

	if (restitution != -1.0f)
	{
		fixture.restitution = restitution;//restitution value can range between 0 and 1 in normal conditions. It determines the bounciness
	}
	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;


	return pbody;
}


PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, float restitution)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	if (restitution != -1.0f)
	{
		fixture.restitution = restitution;//restitution value can range between 0 and 1 in normal conditions. It determines the bounciness
	}
	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateKicker(int pivotX, int pivotY, int x1, int y1, int width, int height)
{
	// Creation of the static ball (needed for joint)
	b2BodyDef ball;
	ball.type = b2_staticBody;
	ball.position.Set(PIXEL_TO_METERS(pivotX), PIXEL_TO_METERS(pivotY));

	b2Body* ball_joint = world->CreateBody(&ball);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(5) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	ball_joint->CreateFixture(&fixture);

	//Creation of the kicker
	b2BodyDef kicker;
	kicker.type = b2_dynamicBody;
	kicker.position.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));

	b2Body* kicker_body = world->CreateBody(&kicker);
	b2PolygonShape kicker_1;
	kicker_1.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture1;
	fixture1.shape = &kicker_1;
	fixture1.density = 1.0f;

	kicker_body->CreateFixture(&fixture1);

	PhysBody* pbody = new PhysBody();
	pbody->body = kicker_body;
	kicker_body->SetUserData(pbody);
	pbody->width = (width * 0.5) + 2;
	pbody->height = height * 0.5;


	//creation of the prismatic joint
	b2PrismaticJointDef jointDef;
	b2Vec2 worldAxis(0.0f, 1.0f);
	jointDef.Initialize(ball_joint, kicker_body, ball_joint->GetWorldCenter(), worldAxis);
	jointDef.lowerTranslation = -1.5f;
	jointDef.upperTranslation = 0.0f;
	jointDef.enableLimit = true;
	jointDef.maxMotorForce = 30.0f;
	jointDef.motorSpeed = 0.0f;
	jointDef.enableMotor = true;	kicker_joint = (b2PrismaticJoint*)world->CreateJoint(&jointDef);

	App->scene_intro->kickers.add(pbody);

	return pbody;

}

// 
update_status ModulePhysics::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return UPDATE_CONTINUE;

	// This will iterate all objects in the world and draw the circles
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				b2Vec2 mousepos = { PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()) };

				if (f->TestPoint(mousepos))
				{
					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = b;
					def.target = mousepos;
					def.dampingRatio = 0.5f;
					def.frequencyHz = 2.0f;
					def.maxForce = 100.0f * b->GetMass();

					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
				}
			}
		}
	}

	// Joint Debug on ball Logic ---------------------------

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && mouse_joint != nullptr)
	{
		mouse_joint->SetTarget({ PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()) });

		App->renderer->DrawLine(METERS_TO_PIXELS(mouse_joint->GetAnchorA().x), METERS_TO_PIXELS(mouse_joint->GetAnchorA().y),
			METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), 255, 100, 100);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && mouse_joint != nullptr)
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);

	if (physB && physB->listener != NULL && App->scene_intro->frogs.findNode(physA))
	{
		App->scene_intro->score += 100;
		App->audio->PlayFx(5, 0);
		physB->listener->OnCollision(physB, physA);
	}

	if (physB && physB->listener != NULL && (App->scene_intro->leftbumper == physA || App->scene_intro->rightbumper == physA))
	{
		if (App->scene_intro->leftbumper == physA)
			App->scene_intro->isleftbumping = true;
		if (App->scene_intro->rightbumper == physA)
			App->scene_intro->isrightbumping = true;

		App->audio->PlayFx(4, 0);
		physB->listener->OnCollision(physB, physA);
	}

	if (physB && physB->listener != NULL && App->scene_intro->red_sensors.findNode(physA))
	{
		App->scene_intro->score += 100;
		physB->listener->OnCollision(physB, physA);	

		App->scene_intro->num_red_sensors--;

		App->scene_intro->red_sensors.del(App->scene_intro->red_sensors.findNode(physA));
		physA->body->GetFixtureList()->SetSensor(true);
	}
}
