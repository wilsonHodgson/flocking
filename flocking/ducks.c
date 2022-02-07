#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "ducks.h"
#include "globals.h"

Duck* flock;
int flock_size = 10;
Duck leadDuck;

void InitFlock()
{
	flock = (Duck*)malloc(flock_size * sizeof(Duck));

	for (int i=1; i<flock_size; i++){
		flock[i].forwards = (Vector2){(1+ rand() % 2) *(-1), (1+ rand() % 2) * (-1)};
		flock[i].position = (Vector2){rand() % screenWidth, rand() % screenHeight};
		flock[i].velocity = (Vector2){0.f, 0.f};
		flock[i].state = 2; //1+ rand() % 2;
	}

	flock[0].forwards = (Vector2){(1+ rand() % 2) *(-1), (1+ rand() % 2) * (-1)};
	flock[0].position = (Vector2){rand() % screenWidth, rand() % screenHeight};
	flock[0].velocity = (Vector2){0.f, 0.f};
	flock[0].state = 0; //1+ rand() % 2;
	leadDuck = flock[0];
	return;
}

void RenderFlock()
{
	for (int i=0; i<flock_size; i++){
		DrawDuck(flock[i]);
	}
	return;
}

void UpdateFlock()
{
	for (int i=0; i<flock_size; i++){
		UpdateDuck(flock+i);
	}
	return;
}

void DrawDuck(Duck duck)
{
	DrawCircleV(duck.position, 20, MAROON);

	//draw forwards
	
	Vector2 forwards = Vector2Normalize(duck.forwards);
	forwards = Vector2Scale(forwards, 30);

	forwards = Vector2Add(duck.position, forwards);
	DrawLineV(duck.position, forwards, MAROON);
}

void UpdateDuck(Duck* duck)
{
	Duck** neighbours = 0;
	size_t neighbours_size = 0;
	HeapArray data;

	switch(duck->state){
		//lead
		case 0:
			duck->velocity.x = 0;
			duck->velocity.y = 0;
			if (IsKeyDown(KEY_A)) {
				duck->forwards = Vector2Rotate(duck->forwards, -0.05);
			}
			if (IsKeyDown(KEY_D)) {
				duck->forwards = Vector2Rotate(duck->forwards, 0.05);
			}
			break;
		//follow
		case 1:
			duck->velocity = Vector2Subtract(leadDuck.position, duck->position);
			duck->velocity = Vector2Normalize(duck->velocity);
			duck->velocity = Vector2Scale(duck->velocity, 5);
			break;
		//flock
		case 2:
			data = getNeighbours(duck, flock, flock_size);
			neighbours = data.index;
			neighbours_size = data.size;
			duck->velocity = (Vector2){0, 0};
			 
			if (neighbours == 0)
				break;

			Vector2 seperation;
			float alignment = 0;
			Vector2 cohesion = {0, 0};
			for (int i=0; i< neighbours_size; i++)
			{
				Vector2 neighbour_difference = Vector2Subtract(duck->position, neighbours[i]->position);
				seperation = Vector2Add(seperation, neighbour_difference);

				cohesion = Vector2Add(cohesion, neighbour_difference);

				//trying to figure out alignment factor

				if (isLeft(duck->forwards, neighbours[i]->forwards)){
					alignment -= acosf(Vector2DotProduct(duck->forwards, neighbours[i]->forwards)/(Vector2Length(duck->forwards)*Vector2Length(neighbours[i]->forwards)));
				} else {
					alignment += acosf(Vector2DotProduct(duck->forwards, neighbours[i]->forwards)/(Vector2Length(duck->forwards)*Vector2Length(neighbours[i]->forwards)));
				}
			}

			//seperation is meant to be a small field with strong strength: This will likely stop boid black holes from forming
			//cohesion is meant to be a large field with small strength
			//alignment is meant to be a moderate sized field

			cohesion = Vector2Scale(cohesion, 0.001 * (float)neighbours_size);
			seperation = Vector2Scale(seperation, 0.001);
			alignment = alignment/neighbours_size/50;

			duck->velocity = Vector2Add(seperation, cohesion);
			duck->forwards = Vector2Rotate(duck->forwards, alignment);
			duck->velocity = Vector2Scale(duck->velocity, 0.0001);
			break;

	}

	duck->position = Vector2Add(duck->position, duck->velocity);
	duck->position = Vector2Add(duck->position, duck->forwards);

	//loop screen edges
	duck->position.y = (int)duck->position.y % screenHeight;
	duck->position.x = (int)duck->position.x % screenWidth;

	if (duck->position.y < 0)
		duck->position.y += screenHeight;

	if (duck->position.x < 0)
		duck->position.x += screenWidth;
}

HeapArray getNeighbours(Duck* self, Duck* flock, int size)
{

	 Duck** neighbours = malloc(sizeof(Duck*));
	 size_t n_size = 1;
	 int count = 1;

	 for (int i = 0; i < size; i++){
		if (flock+i == self)
			continue;
		if (!CheckCollisionPointCircle(self->position, flock[i].position, 250.0))
			continue;

		if (count >= n_size){
			neighbours = (Duck**)realloc(neighbours, sizeof(Duck*) * n_size * 2);
			n_size *= 2;
		}

		neighbours[count-1] = (Duck*)flock+i;
		count++;
	 }

	 neighbours = (Duck**)realloc(neighbours, sizeof(Duck*)*count);

	 HeapArray out = {1, 0};
	 if (count <= 1)
		 return out;

	 out.index = neighbours;
	 out.size = count-1;

	 return out;
}

bool isLeft(Vector2 a, Vector2 b)
{
	a = Vector2Rotate(a, 90);
	float dot_product = Vector2DotProduct(a, b);

	if (dot_product > 0) {
		return false;
	} else if (dot_product < 0) {
		return true;
	}
	return false;
}
