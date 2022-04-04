#include "raylib.h"
typedef struct Duck { Vector2 forwards; Vector2 position; Vector2 velocity; float maxForce; float maxSpeed; Vector2 destination; int state; } Duck;
typedef struct HeapArray { size_t size; void* index; } HeapArray;

void DrawDuck();
void UpdateDuck(Duck*, double);
Vector2 Seek(Duck, Vector2);

void InitFlock();
void RenderFlock();
void UpdateFlock(double);

HeapArray getNeighbours(Duck*, Duck*, int, float);
bool isLeft(Vector2, Vector2);
