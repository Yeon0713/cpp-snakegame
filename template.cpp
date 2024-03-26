#include "console.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

#define BOARD_SIZE console::SCREEN_WIDTH
#define WALL_VERTICAL_STRING "┃"
#define WALL_HORIZONTAL_STRING "━"
#define WALL_RIGHT_TOP_STRING "┓"
#define WALL_LEFT_TOP_STRING "┏"
#define WALL_RIGHT_BOTTOM_STRING "┛"
#define WALL_LEFT_BOTTOM_STRING "┗"
#define SNAKE_STRING "■"
#define APPLE_STRING "●"
#define FRAME_RATE 60
#define MOVE_DELAY_MS (1000 / FRAME_RATE)

int x = BOARD_SIZE / 2;
int y = BOARD_SIZE / 2;
int x_apple;
int y_apple;
int tailX[100];
int tailY[100];
int tail_length = 0;
int dx = 0; // 뱀의 x축 이동 방향
int dy = 0; // 뱀의 y축 이동 방향

void printBoard() {
    // 보드의 가로 길이
    int boardWidth = BOARD_SIZE + 2;

    // 첫 번째 가로 벽 출력
    std::cout << WALL_LEFT_TOP_STRING;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << WALL_HORIZONTAL_STRING;
    }
    std::cout << WALL_RIGHT_TOP_STRING << std::endl;

    // 보드 내부 출력
    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << WALL_VERTICAL_STRING << std::string(BOARD_SIZE, ' ') << WALL_VERTICAL_STRING << std::endl;
    }

    // 마지막 가로 벽 출력
    std::cout << WALL_LEFT_BOTTOM_STRING;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << WALL_HORIZONTAL_STRING;
    }
    std::cout << WALL_RIGHT_BOTTOM_STRING << std::endl;
}

void eraseSnake(int x, int y) {
    // 머리 위치를 지운다
    console::draw(x, y, " ");

    // 꼬리 위치를 지운다
    for (int i = 0; i < tail_length; ++i) {
        console::draw(tailX[i], tailY[i], " ");
    }
}

void handleInput() {
    if (console::key(console::K_LEFT) && dx == 0) {
        dx = -1;
        dy = 0;
    }
    if (console::key(console::K_RIGHT) && dx == 0) {
        dx = 1;
        dy = 0;
    }
    if (console::key(console::K_UP) && dy == 0) {
        dx = 0;
        dy = -1;
    }
    if (console::key(console::K_DOWN) && dy == 0) {
        dx = 0;
        dy = 1;
    }
}

void restrictInScreen() {
    // x, y 위치를 화면의 최대 크기에서 벗어나지 않게 한다.
    if (x < 0)
        x = 0;
    if (x >= console::SCREEN_WIDTH)
        x = console::SCREEN_WIDTH - 1;
    if (y < 0)
        y = 0;
    if (y >= console::SCREEN_HEIGHT)
        y = console::SCREEN_HEIGHT - 1;
}

void drawStar() {
    // x, y 위치에 *을 그린다.
    console::draw(x, y, SNAKE_STRING);
}

bool appleCreated = false; // 사과가 생성되었는지 여부를 나타내는 변수

void createApple() {
    if (!appleCreated) { // 사과가 아직 생성되지 않았다면
        do {
            x_apple = rand() % BOARD_SIZE;
            y_apple = rand() % BOARD_SIZE;
        } while (x == x_apple && y == y_apple); // 뱀의 머리 위치와 사과 위치가 겹치지 않도록 반복하여 생성

        console::draw(x_apple, y_apple, APPLE_STRING);
        appleCreated = true; // 사과가 생성되었음을 표시
    }
}

void moveSnake() {
    // 뱀의 머리 위치를 이동
    x += dx;
    y += dy;
    if (tail_length > 0) {
        for (int i = tail_length - 1; i > 0; --i) {
            tailX[i] = tailX[i - 1];
            tailY[i] = tailY[i - 1];
        }
        tailX[0] = x;
        tailY[0] = y;
    }

    // 뱀의 머리가 사과를 먹었을 경우
    if (x == x_apple && y == y_apple) {
        appleCreated = false; // 사과 생성 상태 초기화
        tail_length++; // 꼬리 길이 증가
    }

    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        // 벽에 닿았으므로 게임 종료
        console::draw((BOARD_SIZE - 9) / 2, BOARD_SIZE / 2, "Game Over!");
        exit(0); // 프로그램 종료
    }
}

void game() {
    // 콘솔 라이브러리를 초기화한다.
    console::init();

    printBoard();

    auto lastFrameTime = clock();

    while (true) {
        auto currentTime = clock();
        auto elapsedTime = (currentTime - lastFrameTime) * 1000 / CLOCKS_PER_SEC; // 경과 시간(ms) 계산

        if (elapsedTime < MOVE_DELAY_MS) {
            continue; // 현재 프레임에서의 경과 시간이 지정된 딜레이보다 작으면 다음 루프로 넘어감
        }

        // 현재 프레임에서의 경과 시간이 지정된 딜레이보다 크거나 같으면 게임 로직 실행
        lastFrameTime = currentTime;

        // 게임 로직
        eraseSnake(x, y);
        handleInput();
        restrictInScreen();
        drawStar();
        createApple();
        moveSnake();

        if (x == x_apple && y == y_apple) {
            appleCreated = false;
            tail_length++;
        }

        // 화면을 갱신하고 다음 프레임까지 대기
        console::wait();
    }
}

int main() {
    // 난수 발생기 초기화
    srand(time(NULL));

    game();

    return 0;
}