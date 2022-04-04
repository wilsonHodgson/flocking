#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "time.h"

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
		flock[i].velocity = Vector2Zero();
		flock[i].destination = flock[i].forwards;

		flock[i].maxForce = 5;
		flock[i].maxSpeed = 4;
		flock[i].state = 2; //1+ rand() % 2;
	}

	//init the leader
	flock[0].forwards = (Vector2){(1+ rand() % 2) *(-1), (1+ rand() % 2) * (-1)};
	flock[0].position = (Vector2){rand() % screenWidth, rand() % screenHeight};
	flock[0].velocity = Vector2Zero();
	flock[0].destination = Vector2Zero();

	flock[0].maxForce = 5;
	flock[0].maxSpeed = 4;
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

void UpdateFlock(double dt)
{
	for (int i=0; i<flock_size; i++){
		UpdateDuck(flock+i, dt);
	}
	return;
}

void DrawDuck(Duck duck, double dt)
{
	if (duck.state == 0) {
		DrawCircleV(duck.position, 20, GREEN);
	} else if (duck.state == 1) {
		DrawCircleV(duck.position, 20, BLUE);
	}else {
		DrawCircleV(duck.position, 20, MAROON);
	}

	//draw forwards
	
	Vector2 forwards = Vector2Normalize(duck.forwards);
	forwards = Vector2Scale(forwards, 30);

	forwards = Vector2Add(duck.position, forwards);
	DrawLineV(duck.position, forwards, MAROON);
}

void UpdateDuck(Duck* duck, double dt)
{
	leadDuck = flock[0];
	Duck** neighbours;
	Duck** near_neighbours;
	Duck** far_neighbours;
	neighbours = near_neighbours = far_neighbours = 0;

	size_t neighbours_size, near_size, far_size;
	neighbours_size = far_size = near_size = 0;
	HeapArray data;

	switch(duck->state){
		//lead
		case 0:
			duck->velocity = Vector2Zero();
			if (IsKeyDown(KEY_A)) {
				duck->forwards = Vector2Rotate(duck->forwards, -0.05);
			}
			if (IsKeyDown(KEY_D)) {
				duck->forwards = Vector2Rotate(duck->forwards, 0.05);
			}
			duck->velocity = Vector2Scale(duck->forwards, 5);
			break;
		//follow
		case 1:
			duck->velocity = Vector2Subtract(leadDuck.position, duck->position);
			duck->velocity = Vector2Normalize(duck->velocity);
			duck->velocity = Vector2Scale(duck->velocity, 5);
			break;
		//flock
		case 2:
			duck->destination = leadDuck.position;

			// I need different sized neighbours for different force measurements
			data = getNeighbours(duck, flock, flock_size, 300.0f);
			neighbours = data.index;
			neighbours_size = data.size;

			data = getNeighbours(duck, flock, flock_size, 55.0f);
			near_neighbours = data.index;
			near_size = data.size;

			data = getNeighbours(duck, flock, flock_size, 600.0f);
			far_neighbours = data.index;
			far_size = data.size;

			//float alignment = 0;
			Vector2 separation = Vector2Zero();
			Vector2 cohesion = Vector2Zero();
			Vector2 average_heading = Vector2Normalize(duck->forwards);
			for (int i=0; i< neighbours_size; i++)
			{
				if (neighbours == 0)
					break;

				average_heading = Vector2Add(average_heading, Vector2Normalize(neighbours[i]->velocity));
			}
			average_heading = Vector2Scale(average_heading, ((float)1)/(neighbours_size));
/*
			if (isLeft(duck->forwards, average_heading)){
				alignment -= acosf(Vector2DotProduct(duck->forwards, average_heading)/(Vector2Length(duck->forwards)*Vector2Length(average_heading)));
			} else {
                        	alignment += acosf(Vector2DotProduct(duck->forwards, average_heading)/(Vector2Length(duck->forwards)*Vector2Length(average_heading)));
			}
*/
			Vector2 desired_velocity = Vector2Scale(average_heading, duck->maxSpeed);
			Vector2 heading_force = Vector2Subtract(desired_velocity, duck->velocity);
			heading_force = Vector2Scale(heading_force, duck->maxForce /duck->maxSpeed);

			for (int i=0; i< near_size; i++)
			{
				if (near_neighbours == 0)
					break;
				Vector2 near_neighbour_difference = Vector2Subtract(near_neighbours[i]->position, duck->position);
				separation = Vector2Add(separation, Vector2Scale(near_neighbour_difference, 1));
			}

			for (int i=0; i< far_size; i++)
			{
				if (far_neighbours == 0)
					break;
				//something fishy is up with cohesion, because it's always prevailing over separation
				//both cohesion and separation are pointing in the same direction
				//Vector2 far_neighbour_difference = Vector2Subtract(duck->position, far_neighbours[i]->position);
				cohesion = Vector2Add(cohesion, far_neighbours[i]->position);
			}
			cohesion = Vector2Scale(cohesion, 1/((float)far_size * 40));
			cohesion = Seek(*duck, cohesion);

			//separation is meant to be a small field with strong strength: This will likely stop boid black holes from forming
			//cohesion is meant to be a large field with small strength
			//alignment is meant to be a moderate sized field

			duck->forwards = Vector2Normalize(average_heading);

			duck->velocity = Vector2Add(duck->velocity, separation);
			duck->velocity = Vector2Add(duck->velocity, cohesion);
			duck->velocity = Vector2Add(duck->velocity, heading_force);

			Vector2 seek = Seek(*duck, duck->destination);
			duck->velocity = Vector2Add(duck->velocity, seek);

			printf("cohesion: {%.6f, %.6f} \n", cohesion.x, cohesion.y);
			printf("separation: {%.6f, %.6f} \n", separation.x, separation.y);
			printf("average heading: %.6f, %.6f \n", average_heading.x, average_heading.y);
			break;

	}

	duck->position = Vector2Add(duck->position, Vector2Scale(duck->velocity, ((float)1)/4));

	//loop screen edges
	if (duck->position.y > screenHeight)
		duck->position.y = (int)duck->position.y % screenHeight;
	if (duck->position.x > screenWidth)
		duck->position.x = (int)duck->position.x % screenWidth;

	if (duck->position.y < 0)
		duck->position.y += screenHeight;

	if (duck->position.x < 0)
		duck->position.x += screenWidth;
}

HeapArray getNeighbours(Duck* self, Duck* flock, int size, float radius)
{

	 Duck** neighbours = malloc(sizeof(Duck*));
	 size_t n_size = 1;
	 int count = 1;

	 for (int i = 0; i < size; i++){
		if (flock+i == self)
			continue;
		if (!CheckCollisionPointCircle(self->position, flock[i].position, radius))
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

Vector2 Seek(Duck duck, Vector2 destination) {
	Vector2 desired = Vector2Subtract(destination, duck.position);
	desired = Vector2Scale(desired, duck.maxSpeed/Vector2Length(desired));

	Vector2 force_change = Vector2Subtract(desired, duck.velocity);
	return Vector2Scale(force_change, duck.maxForce/duck.maxSpeed);
}
