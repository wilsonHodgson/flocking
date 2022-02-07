#include "raylib.h"

void DrawDuck();
void UpdateDuck();

void InitFlock();
void RenderFlock();
void UpdateFlock();

typedef struct Duck { Vector2 forwards; Vector2 position; Vector2 velocity; int state; } Duck;
typedef struct HeapArray { size_t size; void* index; } HeapArray;

HeapArray getNeighbours(Duck*, Duck*, int);
bool isLeft(Vector2, Vector2);
