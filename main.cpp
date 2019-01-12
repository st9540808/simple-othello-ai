#include <iostream>
#include <random>
#include "othello.h"
#include "search.h"

double test(void)
{
    const int play_count = 10000;
    int win_count = 0;

    color player = BLACK;
    // #pragma omp parallel for
    for (int i = 0; i < 5000; i++) {
        Othello othello;
        if (othello.ai_play_with_rand(player))
            // #pragma omp atomic
            win_count++;
    }

    player = WHITE;
    // #pragma omp parallel for
    for (int i = 0; i < 5000; i++) {
        Othello othello;
        if (othello.ai_play_with_rand(player))
            // #pragma omp atomic
            win_count++;
    }

    return (double) win_count / play_count;
}

int main(int argc, char const *argv[])
{
    Othello othello;
    // othello.ai_self_play();
    othello.play_with_ai(BLACK);

    // std::printf("win probability: %f ", test());
    return 0;
}
