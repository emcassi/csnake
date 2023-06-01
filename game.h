// Game.h

#define WIDTH 800
#define HEIGHT 600
#define TITLE "Snake"

#define TILE_SIZE 20

#define SNAKE_COLOR GREEN

struct Snake {
	int x, y;
};

struct Food {
	int x, y;
};

struct SnakeNode {
	struct Snake snake;
	struct SnakeNode* next;
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

struct Food* food;

bool dead = false;
int score = 0;

int currentDir;

struct SnakeNode* head;

const char* gameOverMessage = "GAME OVER";
const char* deetsMessage = "Press Space to Retry";

// General methods
void InitValues();
void DrawBoard();
void Kill();

// Food methods
struct Food GenNewFood();
void DrawFood(struct Food food);

// Snake methods
struct SnakeNode* AddSnakeBit(struct Snake bit);
void AddToTail(struct SnakeNode* head, struct Snake newBit);
int GetCount(struct SnakeNode* head);
void EmptyList(struct SnakeNode* head);
void MoveSnake(struct SnakeNode* head, int dir);
void DrawSnakeBit(struct SnakeNode* node);
void DrawSnake(struct SnakeNode* head);
bool CheckIfOutOfBounds(struct SnakeNode* head, int dir);
bool CheckIfHittingSelf(struct SnakeNode* head);