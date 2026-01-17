#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#define width 30
#define height 20
#define max_length (width * height - 5)

typedef struct position{
    int x, y;
    struct position *pre;
    struct position *next;
} Point;

// 注：全局变量定义后无需再向函数中传递相应参数
Point food;                 // 食物坐标
Point *snake;               // 蛇头指针
Point *tail;                // 蛇尾指针
Point *barrier_1;           // 障碍物1指针（横向移动）
Point *barrier_2;           // 障碍物2指针（纵向移动）
Point *barrier_3;           // 障碍物3指针（斜向移动）
char direction;             // 运动方向（WASD）
int length = 1;             // 蛇身长度
int score = 0;              // 统计分数
int game_over = 0;          // 游戏结束（0为未结束，1为失败，2为成功，3为异常）
int difficulty = 2;         // 游戏难度（1为简单，2为普通，3为困难，4为地狱）
int self_crash = 2;         // 1为开启自撞, 2为关闭自撞（默认）
int through_wall = 2;       // 1为关闭穿墙模式，2为开启穿墙模式（默认）

int init_game();            // 初始化
void create_food();         // 放置食物
int create_barrier();       // 生成障碍
void clear_barrier();       // 清除障碍
void directions();          // 更新方向
int draw_map();             // 画地图
int is_food_conflict();     // 食物刷新辅助函数（保证食物不会刷到蛇身上）
int is_barrier_conflict();  // 障碍刷新辅助函数（保证障碍不会刷到蛇身上且障碍之间不会重叠）
int is_eating();            // 判断是否吃到食物
int moving_snake();         // 移动蛇身，同时判断是否吃到食物（若吃到，再生成食物）
void moving_barrier();      // 移动障碍
int is_fail();              // 判断游戏是否失败
int is_success();           // 判断游戏是否胜利

int init_game(){
    game_over = 0;
    length = 1;
    snake = (Point *)malloc(sizeof(Point));
    if (snake == 0){
        return 0;
    }
    barrier_1 = barrier_2 = barrier_3 = 0;
    tail = snake;
    snake->x = width / 2;
    snake->y = height / 2;
    snake->pre = 0;
    snake->next = 0;
    direction = 's';
    draw_map();
    create_food();
    return 1;
}

void directions(){
    if (_kbhit()){              // _kihit() 用于识别是否进行键盘操作
        char key = _getch();    // _getch() 无需按下 '\n' 就会读取
        switch (key){
            case 'w':
                direction = 'w';
                break;
            case 'a':
                direction = 'a';
                break;
            case 's':
                direction = 's';
                break;
            case 'd':
                direction = 'd';
                break;
            default:
                break;
        }
    }
    return;
}

void create_food(){
    do {
        food.x = rand() % width;    // 在 [0, width - 1] 中随机生成食物横坐标
        food.y = rand() % height;   // 在 [0, height - 1] 中随机生成食物纵坐标
    } while (is_food_conflict());        // 如果生成在了蛇身上，则不断重新生成（妙哉）
    return;
}

int is_food_conflict(){
    Point *run = snake;
    while (run != NULL) {
        if (run->x == food.x && run->y == food.y){
            return 1;
        }
        run = run->next;
    }
    if (barrier_1 != 0 && food.x == barrier_1->x && food.y == barrier_1->y){
        return 1;
    }
    if (barrier_2 != 0 && food.x == barrier_2->x && food.y == barrier_2->y){
        return 1;
    }
    if (barrier_3 != 0 && food.x == barrier_3->x && food.y == barrier_3->y){
        return 1;
    }
    return 0;
}

int is_barrier_conflict(){
    Point *run = snake;
    while (run != 0){
        if (barrier_1 != 0 && run->x == barrier_1->x && run->y == barrier_1->y){
            return 1;
        }
        if (barrier_2 != 0  && run->x == barrier_2->x && run->y == barrier_2->y){
            return 1;
        }
        if (barrier_3 != 0  && run->x == barrier_3->x && run->y == barrier_3->y){
            return 1;
        }
        run = run->next;
    }
    if (barrier_1 != 0  && barrier_2 != 0  && barrier_1->x == barrier_2->x && barrier_1->y == barrier_2->y){
        return 1;
    }
    if (barrier_1 != 0  && barrier_3 != 0  && barrier_1->x == barrier_3->x && barrier_1->y == barrier_3->y){
        return 1;
    }
    if (barrier_2 != 0  && barrier_3 != 0  && barrier_2->x == barrier_3->x && barrier_2->y == barrier_3->y){
        return 1;
    }
    return 0;
}

int create_barrier(){
    barrier_1 = (Point *)malloc(sizeof(Point));
    barrier_2 = (Point *)malloc(sizeof(Point));
    barrier_3 = (Point *)malloc(sizeof(Point));
    if (barrier_1 == 0 || barrier_2 == 0 || barrier_3 == 0){
        return 0;
    }
    barrier_1->next = 0;
    barrier_2->next = 0;
    barrier_3->next = 0;
    barrier_1->pre = 0;
    barrier_2->pre = 0;
    barrier_3->pre = 0;
    do {
        barrier_1->x = rand() % (width);
        barrier_1->y = rand() % (height);
        barrier_2->x = rand() % (width);
        barrier_2->y = rand() % (height);
        barrier_3->x = rand() % (width);
        barrier_3->y = rand() % (height);
    } while (is_barrier_conflict());
    return 1;
}

void clear_barrier(){
    free(barrier_1);
    free(barrier_2);
    free(barrier_3);
    return;
}

// 本函数中无需调整 game_over 数值（main 函数中主动设置）
int is_fail(){
    if(through_wall == 1){
        if (snake->x <= -1 || snake->x >= width || snake->y <= -1 || snake->y >= height){
            return 1;
        }
    }

    if (self_crash == 1){
        int crash_self = 0;
        Point *run = snake;
        if (length >= 2){
            for (int i = 0; i < length - 1; i++){
                run = run->next;
                if (run->x == snake->x && run->y == snake->y){
                    crash_self = 1;
                }
            }
        }
        if (crash_self == 1){
            return 1;
        }
    }

    if (barrier_1 != 0 && snake->x == barrier_1->x && snake->y == barrier_1->y){
        return 1;
    }
    if (barrier_2 != 0 && snake->x == barrier_2->x && snake->y == barrier_2->y){
        return 1;
    }
    if (barrier_3 != 0 && snake->x == barrier_3->x && snake->y == barrier_3->y){
        return 1;
    }
    return 0;
}

// 本函数中无需调整 game_over 数值（main 函数中主动设置）
int is_success(){
    if (length >= max_length){
        return 1;
    }
    return 0;
}

int is_eating(){
    if (snake->x == food.x && snake->y == food.y){
        return 1;
    }
    return 0;
}

int moving_snake(){
    int new_posx = snake->x;
    int new_posy = snake->y;
    switch (direction){
        case 'w':
            new_posy--;
            break;
        case 'a':
            new_posx--;
            break;
        case 's':
            new_posy++;
            break;
        case 'd':
            new_posx++;
            break;
    }
    if (through_wall == 2){
        if (new_posx <= -1){
            new_posx = width - 1;
        }
        if (new_posy <= -1){
            new_posy = height - 1;
        }
        if (new_posx >= width){
            new_posx = 0;
        }
        if (new_posy >= height){
            new_posy = 0;
        }
    }

    Point *new_snake_head = (Point *)malloc(sizeof(Point));
    if (new_snake_head == 0){
        return 3;
    }
    new_snake_head->x = new_posx;
    new_snake_head->y = new_posy;
    new_snake_head->pre = 0;
    new_snake_head->next = snake;
    snake->pre = new_snake_head;
    snake = new_snake_head;
    length++;

    if (is_eating()){
        // 不必再去删除最后一个节点了
        score++;
        create_food();
        clear_barrier();
        if (!create_barrier()){
            return 3;
        }
        return 1;
    }

    length--;

    Point *old_tail = tail;
    if (tail->pre != 0){
        tail = tail->pre;
        free(old_tail);
        tail->next = 0;
    }
    // 如果蛇长（实际长度）为 1（理论上不会进入该分支）
    else{
        snake = 0;
    }
    return 1;
}

void moving_barrier(){
    if (barrier_1 != 0){
        barrier_1->x = (barrier_1->x + 1) % width;
    }
    if (barrier_2 != 0){
        barrier_2->y = (barrier_2->y + 1) % height;
    }
    if (barrier_3 != 0){
        barrier_3->x = (barrier_3->x + 1) % width;
        barrier_3->y = (barrier_3->y + 1) % height;
    }
    return;
}

int draw_map(){
    system("cls");  // 清屏
    // 等价于打印一个空心矩形，边框为 #
    for (int i = 0; i < width + 2; i++){
        printf("#");
    }
    printf("\n");
    for (int y = 0; y < height; y++){
        printf("#");
        for (int x = 0; x < width; x++){
            Point *run = snake;
            int printed = 0;    // 判断当前格是否为蛇身
            while (run != 0){
                if (run->x == x && run->y == y){
                    if (run == snake){
                        printf("@");
                        printed = 1;
                        break;
                    }
                    else{
                        printf("o");
                        printed = 1;
                        break;
                    }
                }
                run = run->next;
            }
            if (printed == 0){
                if (food.x == x && food.y == y){
                    printf("*");
                }
                else if (barrier_1 != 0 && barrier_1->x == x && barrier_1->y == y){
                    printf("X");
                }
                else if (barrier_2 != 0 && barrier_2->x == x && barrier_2->y == y){
                    printf("X");
                }
                else if (barrier_3 != 0 && barrier_3->x == x && barrier_3->y == y){
                    printf("X");
                }
                else{
                    printf(" ");
                }
            }
        }
        printf("#\n");
    }
    for (int i = 0; i < width + 2; i++){
        printf("#");
    }
    printf("\n");
    if (self_crash == 1 && through_wall == 1){
        printf("Difficulty: %d; Points: %d\nCrashing self dies.\nCannot go through the walls.\n", difficulty, score);
    }
    else if (self_crash == 2 && through_wall == 1){
        printf("Difficulty: %d; Points: %d\nCrashing self keeps alive.\nCannot go through the walls.\n", difficulty, score);
    }
    else if (self_crash == 1 && through_wall == 2){
        printf("Difficulty: %d; Points: %d\nCrashing self dies.\nGo through the walls.\n", difficulty, score);
    }
    else{
        printf("Difficulty: %d; Points: %d\nCrashing self keeps alive.\nGo through the walls.\n", difficulty, score);
    }
    return 1;
}

int main(){
    srand((unsigned int)time(NULL));    // 初始化随机种子
    char empty; // 用于清空多余的输入操作
    int difficulty_speed;
    printf("Please choose the difficulty:\n1.Easy\n2.Normal [Default]\n3.Hard\n4.Hell\n");
    printf("Note: Your Basic Scores = Points * cbrt(Difficulty).\n");
    difficulty = getchar() - '0';
    printf("\n");
    if (difficulty == 1) difficulty_speed = 180;
    else if (difficulty == 3) difficulty_speed = 50;
    else if (difficulty == 4) difficulty_speed = 10;
    else {
        difficulty = 2;
        difficulty_speed = 100;
    }

    while ((empty = getchar()) != '\n');    // 清空多余的输入
    printf("Please choose your mode:\n1.Crashing self dies.\n2.Crashing self keeps alive. [Default]\n");
    printf("Note: If you choose Mode 1, Your Final Scores = 1.16 * Basic Scores.\n");
    self_crash = getchar() - '0';
    printf("\n");
    if (self_crash != 1 && self_crash != 2){
        self_crash = 2;
    }

    while ((empty = getchar()) != '\n');    // 清空多余的输入
    printf("Please choose your mode:\n1.You cannot go through the walls.\n2.You can go through the walls. [Default]\n");
    printf("Note: If you choose Mode 1, Your Final Scores = 1.16 * Basic Scores.\n");
    through_wall = getchar() - '0';
    printf("\n");
    if (through_wall != 1 && through_wall != 2){
        through_wall = 2;
    }

    int move_result;
    int init_result = init_game();
    if (init_result == 0){
        printf("ERROR\n");
        return 0;
    }
    if (create_barrier() == 0){
        printf("ERROR!\n");
        return 0;
    }
    while (game_over == 0){
        directions();
        move_result = moving_snake();
        if (move_result == 3){
            game_over = 3;
            break;
        }
        if (is_success()){
            game_over = 2;
            break;
        }
        if (is_fail()){
            game_over = 1;
            draw_map();
            break;
        }
        moving_barrier();
        if (is_fail()){
            game_over = 1;
            draw_map();
            break;
        }
        draw_map();
        Sleep(difficulty_speed);
    }
    draw_map();

    // 基础分 = 吃的食物数量 * 难度的立方根
    double final_score = score * cbrt(difficulty);
    // 若选择开启自撞，则分数额外增加 16%（奖励分）
    if (self_crash == 1) {
        final_score *= 1.16;
    }
    // 若选择关闭穿墙，则分数额外增加 16%（奖励分）
    if (through_wall == 1) {
        final_score *= 1.16;
    }

    if (game_over == 1){
        printf("Game Over! Final Scores: %.2lf\n", final_score);
    }
    if (game_over == 2){
        printf("Congratulations!!! You've beaten the game!!!\n");
        printf("Final Scores: %.2lf\n", final_score);
    }
    if (game_over == 3){
        printf("ERROR!\n");
    }

    // 释放内存
    Point *run = snake;
    while (run != 0){
        Point *to_delete = run;
        run = run->next;
        free(to_delete);
        to_delete = 0;
    }
    snake = tail = 0;
    return 0;
}