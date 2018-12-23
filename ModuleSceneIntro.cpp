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
#include "ModuleRenderer3D.h"

//#include <iostream>
#include "glut/glut.h"
#include <string>


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
	Score();
	TimeLeft();

	ball.radius = 2;
	ball.color = White;

	pb_ball = App->physics->AddBody(ball, 0.5f);
	pb_ball->GetTransform(&ball.transform);
	pb_ball->SetPos(0, 2, 0);	
	
	time_left.Start();
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
	time_remaining = time_left.Read();
	
	LOG("time remaining: %d", time_remaining);
	if (time_remaining >= interval && time_remaining != 0)
	{
		p2List_item<Cylinder*>* item = time_list.getFirst();
		for (int i = 0; i < time_list.count() && item->next != nullptr; item = item->next) {
			if (item->data->color.IsBlack())
			{
				item->data->color = Red;
				break;
			}
			else if (item->next->next == nullptr)
			{
				p2List_item<Cylinder*>* item = time_list.getFirst();
				for (int i = 0; i < time_list.count() && item != nullptr; item = item->next) {
					item->data->color = Black;
				}

				p2List_item<Cylinder*>* item2 = cylinders_list1.getFirst();
				for (int i = 0; i < cylinders_list1.count() && item2 != nullptr; item2 = item2->next) {
					item2->data->color = White;
				}

				p2List_item<Cylinder*>* item3 = cylinders_list2.getFirst();
				for (int i = 0; i < cylinders_list2.count() && item3 != nullptr; item3 = item3->next) {
					item3->data->color = White;
				}

				RestartPositions();
			}

		}
		interval += counter;
	}

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
	//player1->arrow.Render();//If Drawn inside player on the second player duplicates the arrow
	//player2->arrow.Render();

	ball.Render();

	p2List_item<Primitive*>* item = map.getFirst();
	for (int i = 0; i < map.count() && item != nullptr; item = item->next) {
		item->data->Render();
	}

	p2List_item<Cylinder*>* item2 = cylinders_list1.getFirst();
	for (int i = 0; i < cylinders_list1.count() && item2 != nullptr; item2 = item2->next) {
		item2->data->Render();
	}

	p2List_item<Cylinder*>* item3 = cylinders_list2.getFirst();
	for (int i = 0; i < cylinders_list2.count() && item3 != nullptr; item3 = item3->next) {
		item3->data->Render();
	}

	p2List_item<Cylinder*>* item4 = time_list.getFirst();
	for (int i = 0; i < time_list.count() && item4 != nullptr; item4 = item4->next) {
		item4->data->Render();
	}

	return ret;
}


void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	time = timer.Read();
	if (time >= 1000)
	{
		if (body1 == goal_player1)
		{
			if (body2 == pb_ball)
			{
				RestartPositions();

				p2List_item<Cylinder*>* item = cylinders_list1.getFirst();
				for (int i = 0; i < cylinders_list1.count() && item->next != nullptr; item = item->next) {
					if (item->data->color.IsWhite())
					{
						item->data->color = Green;
						timer.Start();
						break;
					}
					else if(item->next->next == nullptr)
					{
						p2List_item<Cylinder*>* item = cylinders_list1.getFirst();
						for (int i = 0; i < cylinders_list1.count() && item != nullptr; item = item->next) {
							item->data->color = White;
						}
						timer.Start();
					}
				}
			}
		}
		else if (body1 == goal_player2)
		{
			if (body2 == pb_ball)
			{
				RestartPositions();

				p2List_item<Cylinder*>* item = cylinders_list2.getFirst();
				for (int i = 0; i < cylinders_list2.count() && item != nullptr; item = item->next) {
					if (item->data->color.IsWhite())
					{
						item->data->color = Green;
						timer.Start();
						break;
					}
					else if (item->next->data == nullptr)
					{
						p2List_item<Cylinder*>* item = cylinders_list2.getFirst();
						for (int i = 0; i < cylinders_list2.count() && item != nullptr; item = item->next) {
							item->data->color = White;
						}	
						timer.Start();
					}
				}
			}
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

	createBoost({ 35,0.5,0 }, 0.5, 0.5);
	createBoost({ -35,0.5,0 }, 0.5, 0.5);

}

void ModuleSceneIntro::createBoost(vec3 pos, float radius, float height) {
	Cylinder* boost = new Cylinder(radius, height);
	boost->SetPos(pos.x, pos.y, pos.z);
	boost->SetRotation(90, vec3(0, 0, 1));
	boost->color = Green;
	sensors.add(boost);
	boosts.add(App->physics->AddBody(*boost, 0.0f));
	boosts.getLast()->data->SetSensors();
	boosts.getLast()->data->collision_listeners.add(this);
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

void ModuleSceneIntro::Score()
{
	float x = -12.75;
	int y = 11;
	int z = 98.5;

	for (int i = 0; i < 5; ++i)
	{
		Cylinder* score = new Cylinder(2, 0.75);
		score->SetPos(x, y, z);
		score->SetRotation(90, { 0,1,0 });
		score->color = White;
		cylinders_list1.add(score);
		x += 6.5;
	}

	for (int i = 0; i < 5; ++i)
	{
		x -= 6.5;
		Cylinder* score = new Cylinder(2, 0.75);
		score->SetPos(-x, y, -z);
		score->SetRotation(90, { 0,1,0 });
		score->color = White;
		cylinders_list2.add(score);	
	}
}

void ModuleSceneIntro::TimeLeft()
{
	Cylinder* time1 = new Cylinder(1, 0.75);
	time1->SetPos(4, -0.5, 0);
	time1->SetRotation(90, { 0,0,1 });
	time1->color = Black;
	time_list.add(time1);

	Cylinder* time2 = new Cylinder(1, 0.75);
	time2->SetPos(-4, -0.5, 0);
	time2->SetRotation(90, { 0,0,1 });
	time2->color = Black;
	time_list.add(time2);

	Cylinder* time3 = new Cylinder(1, 0.75);
	time3->SetPos(0, -0.5, 4);
	time3->SetRotation(90, { 0,0,1 });
	time3->color = Black;
	time_list.add(time3);

	Cylinder* time4 = new Cylinder(1, 0.75);
	time4->SetPos(0, -0.5, -4);
	time4->SetRotation(90, { 0,0,1 });
	time4->color = Black;
	time_list.add(time4);

	Cylinder* time = new Cylinder(1, 0.75);
	time->SetPos(0, -0.5, 0);
	time->SetRotation(90, { 0,0,1 });
	time->color = Black;
	time_list.add(time);
}

void ModuleSceneIntro::RestartPositions()
{
	player1->vehicle->SetPos(player1->initialPosition.x, player1->initialPosition.y, player1->initialPosition.z);
	player2->vehicle->SetPos(player2->initialPosition.x, player2->initialPosition.y, player2->initialPosition.z);
	RotateBody(player1->vehicle);
	RotateBody(player2->vehicle);
	pb_ball->SetPos(0, 2, 0);
	pb_ball->SetVelocityZero();
	player1->vehicle->SetVelocityZero();
	player2->vehicle->SetVelocityZero();
}

