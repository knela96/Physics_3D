#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "ModulePlayer.h"
#include "PhysBody3D.h"

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

public:
	
	Sphere ball;
	PhysBody3D* pb_ball;

	ModulePlayer* player1 = new ModulePlayer(App,true);
	ModulePlayer* player2 = new ModulePlayer(App, true);

	p2List<Primitive*> map;
};
