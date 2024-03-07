#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>
#include <string>

#define wTitle "My First game: Snake Game"  

Color lightGreen{173,204,96,255};
Color darkGreen{43,51,24,255};

int cellSize = 30;
int cellCount = 25;
int wLength = cellSize * cellCount;
int wWidth = wLength;
int offset = 75;

double lastUpdateTime = 0;

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - interval >= lastUpdateTime) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool elementInDeque(Vector2 element, std::deque<Vector2> deque) {
    for (auto el : deque) {
        if (Vector2Equals(el, element)) {
            return true;
        }
    }
    return false;
}

struct Snake {

    std::deque<Vector2> body;
    Vector2 _direction;
    bool addSegment = false;
#define head body[0]

public:
    Snake() {
        body = {Vector2{8, 9}, Vector2{7, 9}, Vector2{6, 9}};
        _direction = {1, 0};
        // body.pop_back();
    }

    Vector2 getDirection() {
        return _direction;
    }

    void changeDirection(Vector2 direction) {
        _direction = direction;
    }

    void update() {
        body.push_front(Vector2Add(body[0], _direction));
        if (addSegment) {
            addSegment = false;
        } else {
            body.pop_back();
        }
    }

    void draw() {
        for(auto cell : body) {
            Rectangle segment = {offset + cell.x * cellSize, offset + cell.y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void reset() {
        body = {Vector2{8, 9}, Vector2{7, 9}, Vector2{6, 9}};
        _direction = {1, 0};
    }
};

struct Food {
    Vector2 position;
    Texture2D texture;
public:
    Food(std::deque<Vector2> snakeBody) {
        Image image = LoadImage("graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = generateValidRandomPosition(snakeBody);
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void draw() {
        // DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, darkGreen);
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 generateRandomPosition() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 generateValidRandomPosition(std::deque<Vector2> snakeBody) {
        Vector2 position = generateRandomPosition();
        while (elementInDeque(position, snakeBody)) 
        {
            position = generateRandomPosition();
        }
        return position;
    }
};

struct Game {
    
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = false;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game() {
        InitAudioDevice();
        eatSound = LoadSound("sounds/eat.mp3");
        wallSound = LoadSound("sounds/wall.mp3");
    }
    ~Game() {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void draw() {
        snake.draw();
        food.draw();
        DrawText("Retro Snake", offset - 5, 20, 40,darkGreen);
        std::string scoreText;
        scoreText += "Score: ";
        scoreText += std::to_string(score);
        DrawText(scoreText.c_str(), offset - 5, offset + wWidth + 10, 40,darkGreen);
    }

    void update(){
        if (running) {
            snake.update();
            checkCollisionWithFood();
            checkCollisionWithBorder();
            checkCollisionWithTail();
        }
    }

    void checkCollisionWithFood() {
        if(Vector2Equals(snake.body[0], food.position)) {
            food.position = food.generateValidRandomPosition(snake.body);
            snake.addSegment = true;
            ++score;
            PlaySound(eatSound);
        }
    }

    void checkCollisionWithBorder() {
        bool leftOrRight = snake.head.x == -1 || snake.head.x == cellCount;
        bool topOrBottom = snake.head.y == -1 || snake.head.y == cellCount;
        if (leftOrRight || topOrBottom) {
            PlaySound(wallSound);
            gameOver();
        }
    }

    void checkCollisionWithTail() {
        for (int i = 1; i <= snake.body.size(); i++) {
            if (Vector2Equals(snake.head, snake.body[i])) {
                gameOver();
            }
        }
    }

    void gameOver() {
        // std::cout << "Game Over!" << std::endl;
        snake.reset();
        food.position = food.generateValidRandomPosition(snake.body);
        running = false;
        score = 0;
    }
};

int main() {
    std::cout << "Starting the game..." << std::endl;
    InitWindow(offset * 2 + wLength, offset * 2 + wWidth, wTitle);
    SetTargetFPS(60);
    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();
        game.draw();
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)wLength + 10, (float)wWidth + 10}, 5, darkGreen);

        ClearBackground(lightGreen);

        if (eventTriggered(0.2)) {
            game.update();
        }
        
        if(IsKeyPressed(KEY_UP) && game.snake.getDirection().y != 1) {
            Vector2 newDirection = {0, -1};
            game.snake.changeDirection(newDirection);
            game.running = true;
        }

        if(IsKeyPressed(KEY_RIGHT) && game.snake.getDirection().x != -1) {
            Vector2 newDirection = {1, 0};
            game.snake.changeDirection(newDirection);
            game.running = true;
        }

        if(IsKeyPressed(KEY_DOWN) && game.snake.getDirection().y != -1) {
            Vector2 newDirection = {0, 1};
            game.snake.changeDirection(newDirection);
            game.running = true;
        }

        if(IsKeyPressed(KEY_LEFT) && game.snake.getDirection().x != 1) {
            Vector2 newDirection = {-1, 0};
            game.snake.changeDirection(newDirection);
            game.running = true;
        }

        EndDrawing();
    }    
    return 0;
}