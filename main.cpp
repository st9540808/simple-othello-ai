#include <iostream>
#include <random>
#include "othello.h"
#include "search.h"

int main(int argc, char const *argv[])
{
    const int play_count = 1000;
    int win_count;

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

    std::printf("win probability: %f\n", (float) win_count / play_count);

    // Othello othello;
    // othello.ai_self_play();

    return 0;
}
