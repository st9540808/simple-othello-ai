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
    int _a = 0, _b = 0, _c = 0, _d = 0, _e = 0;
    double min_prob = 1;

    for (int i = 0; i < 100; i++) {
        #pragma omp parallel for num_threads(6) collapse(2)
        for (int a = -1; a <= 1; a++)
        for (int b = -1; b <= 1; b++)
        for (int c = -1; c <= 1; c++)
        for (int d = -1; d <= 1; d++)
        for (int e = -1; e <= 1; e++) {
            double prob = test(a, b, c, d, e);
            if (prob < min_prob) {
                prob = min_prob;
                _a = a;
                _b = b;
                _c = c;
                _d = d;
                _e = e;
            }
            std::printf("win probability: %f (%d, %d, %d, %d, %d)\n", prob, a, b, c, d, e);
        }

        set_init(_a, _b, _c, _d, _e);
    }
    return 0;
}
