#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "Color.h"
#include "PhysVehicle3D.h"
#include "ModulePlayer.h"

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

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	player1->Start(0, 0, 100, 3.14);

	player2->Start(0, 0, -100, 0);



	ball.radius = 2;
	ball.color = Blue;
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

	App->camera->Position.x = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - 15 * player1->vehicle->vehicle->getForwardVector().getX();
	App->camera->Position.y = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + 5 * player1->vehicle->vehicle->getUpAxis();
	App->camera->Position.z = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - 15 * player1->vehicle->vehicle->getForwardVector().getZ();

	float player1_x = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() + 10 * player1->vehicle->vehicle->getForwardVector().getX();
	float player1_z = player1->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() + 10 * player1->vehicle->vehicle->getForwardVector().getZ();

	App->camera->LookAt(vec3(player1_x, 1, player1_z));
	
	App->camera2->Position.x = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - 15 * player2->vehicle->vehicle->getForwardVector().getX();
	App->camera2->Position.y = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + 5 * player2->vehicle->vehicle->getUpAxis();
	App->camera2->Position.z = player2->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - 15 * player2->vehicle->vehicle->getForwardVector().getZ();

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
	return ret;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

