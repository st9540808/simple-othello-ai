#include <iostream>
#include <random>
#include "othello.h"
#include "search.h"

double test(void)
{
    const int play_count = 1000;
    int win_count = 0;

    color player = BLACK;
    for (int i = 0; i < 500; i++) {
        Othello othello;
        if (othello.ai_play_with_rand(player))
            win_count++;
    }

    player = WHITE;
    for (int i = 0; i < 500; i++) {
        Othello othello;
        if (othello.ai_play_with_rand(player))
            win_count++;
    }

    return (double) win_count / play_count;
}

int main(int argc, char const *argv[])
{
    Othello othello;
    othello.ai_self_play();
    // othello.play_with_ai(BLACK);

    // std::printf("win probability: %f ", test());
}
