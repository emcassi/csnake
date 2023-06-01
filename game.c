#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "game.h"

void InitValues() {
	cooldown = 0.2f;
	timer = cooldown;
	struct Snake headBit = { WIDTH / TILE_SIZE / 2, HEIGHT / TILE_SIZE / 2 };
	head = AddSnakeBit(headBit);
	*food = GenNewFood();

	currentDir = EAST;

	dead = false;
	score = 0;

}

void DrawBoard() {
	for (int i = 0; i < WIDTH / TILE_SIZE; i++) {
		DrawLine(i * TILE_SIZE, 2 * TILE_SIZE, i * TILE_SIZE, HEIGHT, DARKGREEN);
	}
	for (int i = 2; i < HEIGHT / TILE_SIZE; i++) {
		DrawLine(0, i * TILE_SIZE, WIDTH, i * TILE_SIZE, DARKGREEN);
	}
}

void Kill() {
	dead = true;
}

struct Food GenNewFood() {
	int width = WIDTH / TILE_SIZE;
	int height = HEIGHT / TILE_SIZE - 2;

	int x = rand() % width;
	int y = rand() % (height) + 2;
	
	struct Food newFood = { x, y };
	return newFood;
}

void DrawFood(struct Food food) {
	int size = (int)(TILE_SIZE * 0.5);

	DrawRectangle(food.x * TILE_SIZE + size / 2, food.y * TILE_SIZE + size / 2, size, size, ORANGE);
}

struct SnakeNode* AddSnakeBit(struct Snake bit) {
	struct SnakeNode* node = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	if (node) {
		struct Snake newSnake = { bit.x, bit.y };
		node->snake = newSnake;
		node->next = 0;
	}
	return node;
}

void AddToTail(struct SnakeNode* head, struct Snake newBit) {
	struct SnakeNode* temp = head;

	while (temp->next) {
		temp = temp->next;
	}

	temp->next = AddSnakeBit(newBit);
}

int GetCount(struct SnakeNode* head) {
	int count = 0;
	struct SnakeNode* current = head;

	while (current) {
		count++;
		current = current->next;
	}

	return count;
}

void EmptyList(struct SnakeNode* head) {
	struct SnakeNode* current = head;
	struct SnakeNode* next;

	while (current) {
		next = current->next;
		free(current);
		current = next;
	}
}

void MoveSnake(struct SnakeNode* head, int dir) {
	struct SnakeNode* temp = head;
	struct Snake prevSnake = head->snake;

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

	while (temp->next) {
		struct Snake currSnake = temp->next->snake;
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

void DrawSnakeBit(struct SnakeNode* node) {
	DrawRectangle(node->snake.x * TILE_SIZE, node->snake.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, SNAKE_COLOR);
}

void DrawSnake(struct SnakeNode* head) {
	struct SnakeNode* temp = head;

	while(temp) {
		DrawSnakeBit(temp);
		if (temp->next)
			temp = temp->next;
		else
			break;
	}
}

bool CheckIfOutOfBounds(struct SnakeNode* head, int dir) {
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

bool CheckIfHittingSelf(struct SnakeNode* head) {
	struct SnakeNode* temp;

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

int main() {
	InitWindow(WIDTH, HEIGHT, TITLE);

	food = (struct Food*)malloc(sizeof(struct Food));
	InitValues();

	int goWidth = MeasureText(gameOverMessage, 50);
	int deetsWidth = MeasureText(deetsMessage, 28);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		DrawSnake(head);
		DrawFood(*food);
		char scoreText[20];
		sprintf_s(scoreText, sizeof(scoreText), "%d", score);
		DrawText(scoreText, WIDTH / 2, 10, 24, WHITE);
		DrawBoard();
		if (dead) {
			DrawText(gameOverMessage, WIDTH / 2 - goWidth / 2, HEIGHT / 2 - 48, 48, YELLOW);
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