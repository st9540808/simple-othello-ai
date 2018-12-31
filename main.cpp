#include <iostream>
#include <random>
#include "othello.h"
#include "search.h"

double test(int a, int b, int c, int d, int e)
{
    const int play_count = 1000;
    int win_count = 0;

    add_coeff(a, b, c, d, e);

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
    // Othello othello;
    // othello.ai_self_play();

    for (int a = -1; a <= 1; a++)
    for (int b = -1; b <= 1; b++)
    for (int c = -1; c <= 1; c++)
    for (int d = -1; d <= 1; d++)
    for (int e = -1; e <= 1; e++)
        std::printf("win probability: %f (%d, %d, %d, %d, %d)\n",
                    test(a, b, c, d, e), a, b, c, d, e);

    return 0;
}
