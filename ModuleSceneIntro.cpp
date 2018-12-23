#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "Color.h"
#include "PhysVehicle3D.h"
#include "ModulePlayer.h"
#include "Color.h"
#include "PhysBody3D.h"
#include "ModulePhysics3D.h"
#include "cmath"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	player1 = new ModulePlayer(App, true);
	player2 = new ModulePlayer(App, true);

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	player1->Start(0, 0, 95, 3.14, PLAYER1, Orange);

	player2->Start(0, 0, -95, 0, PLAYER2, Blue);

	createMap();
	PutSensors();

	ball.radius = 2;
	ball.color = White;
	pb_ball = App->physics->AddBody(ball, 3.0f);
	pb_ball->GetTransform(&ball.transform);
	pb_ball->SetPos(0, 0, 0);
	
	
	
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	player1->CleanUp();
	player2->CleanUp();
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	player1->Update(dt);
	player2->Update(dt);

	pb_ball->GetTransform(&ball.transform);

	App->camera->Position.x = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - 10 * player1->vehicle->vehicle->getForwardVector().getX();
	App->camera->Position.y = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + 5 * player1->vehicle->vehicle->getUpAxis();
	App->camera->Position.z = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - 10 * player1->vehicle->vehicle->getForwardVector().getZ();

	float player1_x = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() + 10 * player1->vehicle->vehicle->getForwardVector().getX();
	float player1_z = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() + 10 * player1->vehicle->vehicle->getForwardVector().getZ();

	App->camera->LookAt(vec3(player1_x, 1, player1_z));
	
	App->camera2->Position.x = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - 10 * player2->vehicle->vehicle->getForwardVector().getX();
	App->camera2->Position.y = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + 5 * player2->vehicle->vehicle->getUpAxis();
	App->camera2->Position.z = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - 10 * player2->vehicle->vehicle->getForwardVector().getZ();

	float player2_x = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() + 10 * player2->vehicle->vehicle->getForwardVector().getX();
	float player2_z = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() + 10 * player2->vehicle->vehicle->getForwardVector().getZ();

	App->camera2->LookAt(vec3(player2_x, 1, player2_z));

	return UPDATE_CONTINUE;
}

bool ModuleSceneIntro::Draw() {
	bool ret = true;
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	ret = player1->Draw();
	ret = player2->Draw();
	ball.Render();

	p2List_item<Primitive*>* item = map.getFirst();
	for (int i = 0; i < map.count() && item != nullptr; item = item->next) {
		item->data->Render();
	}
/*
	p2List_item<Primitive*>* item2 = sensors.getFirst();
	for (int i = 0; i < sensors.count() && item2 != nullptr; item2 = item2->next) {
		item2->data->Render();
	}*/

	return ret;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == goal_player1)
	{
		if (body2 == pb_ball)
		{
			//add score player1 and reset position
			player1->vehicle->SetPos(player1->initialPosition.x, player1->initialPosition.y, player1->initialPosition.z);
			player2->vehicle->SetPos(player2->initialPosition.x, player2->initialPosition.y, player2->initialPosition.z);
			RotateBody(player1->vehicle);
			RotateBody(player2->vehicle);
			pb_ball->SetPos(0, 0, 0);
			pb_ball->SetVelocityZero();
		}
	}

	else if (body1 == goal_player2)
	{
		if (body2 == pb_ball)
		{
			//add score player2 and reset position
			player1->vehicle->SetPos(player1->initialPosition.x, player1->initialPosition.y, player1->initialPosition.z);
			player2->vehicle->SetPos(player2->initialPosition.x, player2->initialPosition.y, player2->initialPosition.z);
			RotateBody(player1->vehicle);
			RotateBody(player2->vehicle);
			pb_ball->SetPos(0, 0, 0);
			pb_ball->Push(0, 0, 0);
			pb_ball->SetVelocityZero();
		}
	}

}

void ModuleSceneIntro::createMap()
{
	//BLUE FIELD

	//Left
	Cube* p = new Cube(30,15,0.1);
	p->color = Orange;
	p->SetPos(-30, 7.5f, 100);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Right
	p = new Cube(30, 15, 0.1);
	p->color = Orange;
	p->SetPos(30, 7.5f, 100);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Center
	p = new Cube(30, 6, 0.1);
	p->color = Orange;
	p->SetPos(0, 12, 100);
	map.add(p);
	App->physics->AddBody(*p,0.0f);

	//GoalCenter
	p = new Cube(30, 9, 0.1);
	p->color = Orange;
	p->SetPos(0, 4.5, 112);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//GoalLeft
	p = new Cube(0.1, 9, 12);
	p->color = Orange;
	p->SetPos(15, 4.5, 106);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//GoalRight
	p = new Cube(0.1, 9, 12);
	p->color = Orange;
	p->SetPos(-15, 4.5, 106);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//GoalTop
	p = new Cube(30, 0.1, 12);
	p->color = Orange;
	p->SetPos(0, 9, 106);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);
	
	//Side
	p = new Cube(0.1, 15, 100);
	p->color = Orange;
	p->SetPos(-45, 7.5f, 50);
	map.add(p);
	App->physics->AddBody(*p,0.0f);

	//Side
	p = new Cube(0.1, 15, 100);
	p->color = Orange;
	p->SetPos(45, 7.5f, 50);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Top
	p = new Cube(90, 0.1, 100);
	p->color = Orange;
	p->SetPos(0, 15, 50);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);


	//ORANGE FIEDLD

	//Side
	p = new Cube(0.1, 15, 100);
	p->color = Blue;
	p->SetPos(-45, 7.5f, -50);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Side
	p = new Cube(0.1, 15, 100);
	p->color = Blue;
	p->SetPos(45, 7.5f, -50);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Left
	p = new Cube(30, 15, 0.1);
	p->color = Blue;
	p->SetPos(-30, 7.5f, -100);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Right
	p = new Cube(30, 15, 0.1);
	p->color = Blue;
	p->SetPos(30, 7.5f, -100);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Center
	p = new Cube(30, 6, 0.1);
	p->color = Blue;
	p->SetPos(0, 12, -100);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//GoalCenter
	p = new Cube(30, 9, 0.1);
	p->color = Blue;
	p->SetPos(0, 4.5, -112);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//GoalLeft
	p = new Cube(0.1, 9, 12);
	p->color = Blue;
	p->SetPos(15, 4.5, -106);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//GoalRight
	p = new Cube(0.1, 9, 12);
	p->color = Blue;
	p->SetPos(-15, 4.5, -106);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//GoalTop
	p = new Cube(30, 0.1, -12);
	p->color = Blue;
	p->SetPos(0, 9, 106);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);

	//Top
	p = new Cube(90, 0.1, 100);
	p->color = Blue;
	p->SetPos(0, 15, -50);
	map.add(p);
	App->physics->AddBody(*p, 0.0f);
}

void ModuleSceneIntro::PutSensors()
{
	Cube* sensor1 = new Cube(30, 9, 0.1);
	sensor1->SetPos(0, 4.5, 103);
	sensor1->color = White;
	sensors.add(sensor1);
	goal_player1 = App->physics->AddBody(*sensor1, 0.0f);
	goal_player1->SetSensors();
	goal_player1->collision_listeners.add(this);
	
	Cube* sensor2 = new Cube(30, 9, 0.1);
	sensor2->SetPos(0, 4.5, -103);
	sensor2->color = White;
	sensors.add(sensor2);
	goal_player2 = App->physics->AddBody(*sensor2, 0.0f);
	goal_player2->SetSensors();
	goal_player2->collision_listeners.add(this);
}

void ModuleSceneIntro::RotateBody(PhysVehicle3D * vehicle)
{
	float x = vehicle->GetPos().x;
	float z = vehicle->GetPos().z;
	float h = sqrt(x*x + z * z);

	float b = x / h;
	float angle = acos(b);
	if (vehicle == player1->vehicle)
		App->physics->rotateVehicle(angle + M_PI/2, vehicle);
	else if (vehicle == player2->vehicle)
		App->physics->rotateVehicle(angle - M_PI / 2, vehicle);
}
