#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "globals.h"
#include "ducks.h"

int main(void)
{

	InitWindow(screenWidth, screenHeight, "Duck Flock");
	SetTargetFPS(60);
	InitFlock();

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		RenderFlock();
		DrawText("This is duck flock.", 10, 10, 20, DARKGRAY);

		EndDrawing();

		UpdateFlock();

	}

	CloseWindow();

	return 0;
}
