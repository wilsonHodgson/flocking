#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "time.h"

#include "globals.h"
#include "ducks.h"

int main(void)
{

	InitWindow(windowWidth, windowHeight, "Duck Flock");
	SetTargetFPS(60);

	Camera3D camera = { 0 };
	camera.position = (Vector3){ 0.0f, 150.0f, 0.0f}; // Camera position
	camera.target = (Vector3){ 0.0f, 0.0f, 0.0f};      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type

	InitFlock();

	clock_t timer;
	timer = clock();

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		RenderFlock();
		DrawText("This is duck flock.", 10, 10, 20, DARKGRAY);


		//BeginMode3D(camera);

		DrawGrid(1000, 1.0f);
		//RenderFlock3D();

		//EndMode3D();
		EndDrawing();

		timer = clock() - timer;
		double dt = ((double)timer)/CLOCKS_PER_SEC;
		UpdateFlock(dt);
		UpdateCamera(&camera, CAMERA_FREE);
	}

	CloseWindow();

	return 0;
}
