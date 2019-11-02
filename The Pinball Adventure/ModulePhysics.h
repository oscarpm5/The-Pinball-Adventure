#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, bool dynamic, float restitution = -1.0f, bool alwaysactive = false);
	PhysBody* CreateRectangle(int x, int y, int width, int height, bool dynamic, float angle = 0.0f, bool sensor = false, float restitution = -1.0f);
	PhysBody* CreateChain(int x, int y, int* points, int size, float restitution = -1.0f);
	PhysBody* CreateFlipper(int x1, int y1, int x2, int y, int width, int height, int left_right);
	PhysBody* CreateKicker(int pivotX, int pivotY, int x1, int y1, int width, int height);
	//PhysBody* CreateBumper(int pivotX, int pivotY, int x1, int y1, int width, int height);


	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

	b2RevoluteJoint* flipper_joint_left;
	b2RevoluteJoint* flipper_joint_right;
	b2PrismaticJoint* kicker_joint;

private:

	bool debug;
	b2World* world;
	b2MouseJoint* mouse_joint;

	b2Body* ground;

	int revolute_joint_speed = 12;

	b2RevoluteJointDef jointDef1;
};