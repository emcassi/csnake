#define WIDTH 800
#define HEIGHT 600
#define TITLE "Snake"

#define TILE_SIZE 20

#define SNAKE_COLOR GREEN

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

struct Snake {
	int x, y;
};

struct Food {
	int x, y;
};

struct SnakeNode {
	Snake snake;
	SnakeNode* next;
};

enum Direction {
	NORTH,
	SOUTH,
	WEST,
	EAST
};

float minCooldown = 0.08f;
float cooldown = 0.2f;
float timer;

Food* food;

bool dead = false;
int score = 0;

Direction currentDir;

SnakeNode* head;


void DrawFood(Food food) {
	int size = (int)(TILE_SIZE * 0.5);

	DrawRectangle(food.x * TILE_SIZE + size / 2, food.y * TILE_SIZE + size / 2, size, size, ORANGE);
}

Food GenNewFood() {
	int width = WIDTH / TILE_SIZE;
	int height = HEIGHT / TILE_SIZE - 2;

	int x = rand() % width;
	int y = rand() % (height) + 2;

	return { x, y };
}

int GetCount(SnakeNode* head) {
	int count = 0;
	SnakeNode* current = head;

	while (current != NULL) {
		count++;
		current = current->next;
	}

	return count;
}

void DrawSnakeBit(SnakeNode* node) {
	DrawRectangle(node->snake.x * TILE_SIZE, node->snake.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, SNAKE_COLOR);
}

void DrawSnake(SnakeNode* head) {
	SnakeNode* temp = head;

	while(temp) {
		DrawSnakeBit(temp);
		if (temp->next)
			temp = temp->next;
		else
			break;
	}
}


SnakeNode* AddSnakeBit(Snake bit) {
	SnakeNode* node = (SnakeNode*)malloc(sizeof(SnakeNode));
	if (node != NULL) {
		Snake newSnake = { bit.x, bit.y };
		node->snake = newSnake;
		node->next = NULL;
	}
	return node;
}

void AddToTail(SnakeNode* head, Snake newBit) {
	SnakeNode* temp = head;
	
	while (temp->next) {
		temp = temp->next;
	}

	temp->next = AddSnakeBit(newBit);
}

bool CheckIfOutOfBounds(SnakeNode* head, Direction dir) {
	// Kill the snake if it hits a wall
	switch (dir) {
	case NORTH:
		if (head->snake.y - 1 < 2)
			return true;
		break;
	case SOUTH:
		if (head->snake.y + 1 >= HEIGHT / TILE_SIZE)
			return true;
		break;
	case WEST:
		if (head->snake.x - 1 < 0)
			return true;
		break;
	case EAST:
		if (head->snake.x + 1 >= WIDTH / TILE_SIZE)
			return true;
		break;
	}

	return false;
}

bool CheckIfHittingSelf(SnakeNode* head) {
	SnakeNode* temp;

	if (head->next) {
		temp = head->next;
		while (temp) {
			if (head->snake.x == temp->snake.x && head->snake.y == temp->snake.y && temp != head->next) {
				return true;
			}
			temp = temp->next;
		}
	}
	return false;
}

void Kill() {
	dead = true;
}

void MoveSnake(SnakeNode* head, Direction dir) {
	SnakeNode* temp = head;
	Snake prevSnake = head->snake;

	if (CheckIfOutOfBounds(head, dir))
	{
		Kill();
		return;
	}
	switch (dir) {
		case NORTH:
			head->snake.y -= 1;
			break;
		case SOUTH:
			head->snake.y += 1;
			break;
		case WEST:
			head->snake.x -= 1;
			break;
		case EAST:
			head->snake.x += 1;
			break;
	}

	if (CheckIfHittingSelf(head)) {
		Kill();
		return;
	}

	while (temp->next != NULL) {
		Snake currSnake = temp->next->snake;
		temp->next->snake = prevSnake;
		prevSnake = currSnake;
		temp = temp->next;
	}

	if (head->snake.x == food->x && head->snake.y == food->y) {
		AddToTail(head, temp->snake);
		*food = GenNewFood();
		cooldown *= 0.95f;
		if (cooldown < minCooldown)
			cooldown = minCooldown;
		score++;
	}
	
}

void EmptyList(SnakeNode* head) {
	SnakeNode* current = head;
	SnakeNode* next;

	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
}

void DrawGrid() {
	for (int i = 0; i < WIDTH / TILE_SIZE; i++) {
		DrawLine(i * TILE_SIZE, 2 * TILE_SIZE, i * TILE_SIZE, HEIGHT, DARKGREEN);
	}
	for (int i = 2; i < HEIGHT / TILE_SIZE; i++) {
		DrawLine(0, i * TILE_SIZE, WIDTH, i * TILE_SIZE, DARKGREEN);
	}
}

void InitValues() {
	cooldown = 0.2f;
	timer = cooldown;

	head = AddSnakeBit({ WIDTH / TILE_SIZE / 2, HEIGHT / TILE_SIZE / 2 });
	*food = GenNewFood();

	currentDir = EAST;

	dead = false;
	score = 0;

}


int main() {
	InitWindow(WIDTH, HEIGHT, TITLE);

	food = (Food*)malloc(sizeof(Food));
	InitValues();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		DrawSnake(head);
		DrawFood(*food);
		char scoreText[20];
		sprintf_s(scoreText, "%d", score);
		DrawText(scoreText, WIDTH / 2, 10, 24, WHITE);
		DrawGrid();
		if (dead) {
			const char* goMessage = "GAME OVER";
			int goWidth = MeasureText(goMessage, 50);
			DrawText(goMessage, WIDTH / 2 - goWidth / 2, HEIGHT / 2 - 48, 48, YELLOW);
			const char* deetsMessage = "Press Space to Retry";
			int deetsWidth = MeasureText(deetsMessage, 28);
			DrawText(deetsMessage, WIDTH / 2 - deetsWidth / 2, HEIGHT / 2 + 100, 26, WHITE);
		}
		EndDrawing();

		if (dead) {
			if (IsKeyPressed(KEY_SPACE))
			{
				EmptyList(head);
				InitValues();
			}
		}
		else {

			if ((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) && currentDir != NORTH)
				currentDir = SOUTH;
			if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && currentDir != SOUTH)
				currentDir = NORTH;
			if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && currentDir != EAST)
				currentDir = WEST;
			if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && currentDir != WEST)
				currentDir = EAST;

			if (timer <= 0) {
				timer = cooldown;
				MoveSnake(head, currentDir);
			}
			else {
				timer -= GetFrameTime();
			}
		}
		
	}
	CloseWindow();

	EmptyList(head);
	free(food);

	return 0;
}