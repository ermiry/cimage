#ifdef GAME_H_
#define GAME_H_
#define N 3
#define WIDTH 1280
#define HEIGHT 720
#define  RUNNING_STATE 0
#define QUIT_STATE 4

typedef struct{
    int page;
    int user;
    int state;
}game_t;

#endif