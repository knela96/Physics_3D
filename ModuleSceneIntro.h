#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "ModulePlayer.h"
#include "PhysBody3D.h"
#include "Timer.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool Draw();
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	void createMap();
	void PutSensors();

	void RotateBody(PhysVehicle3D * vehicle);

	void Score();

public:
	
	Sphere ball;
	PhysBody3D* pb_ball;
	PhysBody3D* goal_player1;
	PhysBody3D* goal_player2;

	ModulePlayer* player1;
	ModulePlayer* player2;

	p2List<Primitive*> map;
	p2List<Primitive*> sensors;

	p2List<Cylinder*> cylinders_list1;
	p2List<Cylinder*> cylinders_list2;

	Timer timer;
	Uint32 time = 0;

	bool goal = false;
};
