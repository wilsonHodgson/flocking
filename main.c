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

	InitWindow(screenWidth, screenHeight, "Duck Flock");
	SetTargetFPS(60);
	InitFlock();

	clock_t timer;
	timer = clock();

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		RenderFlock();
		DrawText("This is duck flock.", 10, 10, 20, DARKGRAY);

		EndDrawing();

		timer = clock() - timer;
		double dt = ((double)timer)/CLOCKS_PER_SEC;
		UpdateFlock(dt);
	}

	CloseWindow();

	return 0;
}
