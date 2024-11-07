// start: 2024-11-7 16:16
// end: 2024-11-7 16:40

#include <deque>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#define M 20
#define N 20
#define L 100

enum dir_t {
  d_up,
  d_down,
  d_left,
  d_right
} curr_dir = d_up;
char array[M][N];
std::pair<int, int> food;
std::deque<std::pair<int, int>> snake;

void draw_wall() {
  for (int i = 0; i < M; i++) {
    array[i][0] = '|';
    array[i][N - 1] = '|';
  }
  for (int j = 0; j < N; j++) {
    array[0][j] = '-';
    array[M - 1][j] = '-';
  }

  array[0][0] = '+';
  array[0][N - 1] = '+';
  array[M - 1][0] = '+';
  array[M - 1][N - 1] = '+';
}

void initialize() {
  for (int i = 0; i < M; i++)
    for (int j = 0; j < N; j++) array[i][j] = ' ';
  draw_wall();

  array[M / 2][N / 2] = '*';
  snake.push_back(std::pair<int, int>{M / 2, N / 2});
}

void move_snake(dir_t dir) {
  static bool is_dead = false;
  if (is_dead) return;

  auto head = snake.front();
  auto tail = snake.back();
  if (food.first == head.first &&
      food.second == head.second) {
    food.first = rand() % (M - 2) + 1;
    food.second = rand() % (N - 2) + 1;
    array[food.first][food.second] = '$';
  } else {
    array[tail.first][tail.second] = ' ';
    snake.pop_back();
  }
  switch (dir) {
  case d_up:
    snake.push_front(
        std::pair<int, int>{head.first - 1, head.second});
    break;
  case d_down:
    snake.push_front(
        std::pair<int, int>{head.first + 1, head.second});
    break;
  case d_left:
    snake.push_front(
        std::pair<int, int>{head.first, head.second - 1});
    break;
  case d_right:
    snake.push_front(
        std::pair<int, int>{head.first, head.second + 1});
    break;
  }
  auto new_head = snake.front();
  array[new_head.first][new_head.second] = '*';

  // check if dead
  if (new_head.first == 0 || new_head.first == M - 1
      || new_head.second == 0 || new_head.second == N - 1)
    is_dead = true;
}

char getch() {
  char c;
  struct termios old, new_scr;
  tcgetattr(0, &old);
  new_scr = old;
  new_scr.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(0, TCSANOW, &new_scr);
  c = getchar();
  tcsetattr(0, TCSANOW, &old);
  return c;
}

bool check_elapsed_gettimeofday() {
  static struct timeval start = {0, 0};
  struct timeval now;

  if (start.tv_sec == 0) {
    gettimeofday(&start, NULL);
    return false;
  }

  gettimeofday(&now, NULL);
  double elapsed =
      (now.tv_sec - start.tv_sec) +
      (now.tv_usec - start.tv_usec) / 1000000.0;

  if (elapsed >= 0.5) {
    start = now; // Reset timer
    return true;
  }
  return false;
}

void *update_screen(void *args) {
  while (true) {
    system("clear");
    for (int i = 0; i < M; i++) {
      for (int j = 0; j < N; j++) printf("%c", array[i][j]);
      printf("\n");
    }
    if (check_elapsed_gettimeofday()) move_snake(curr_dir);
    fflush(stdout);
    usleep(20000);
  }
}

int main() {
  initialize();
  pthread_t pth = {};
  pthread_create(&pth, NULL, update_screen, NULL);

  food.first = rand() % (M - 2) + 1;
  food.second = rand() % (N - 2) + 1;
  array[food.first][food.second] = '$';
  while (true) {
    char ch = getch();
    switch (ch) {
    case 'w': curr_dir = d_up; break;
    case 's': curr_dir = d_down; break;
    case 'a': curr_dir = d_left; break;
    case 'd': curr_dir = d_right; break;
    }
  }
  return 0;
}
