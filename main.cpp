#include <iostream>
#include "othello.h"
#include "search.h"

int main(int argc, char const *argv[])
{
    Othello othello;
    // othello.play();
    // print_bitboard(1ull << alphabeta(othello, 12, BLACK));
    othello.ai_self_play();

    return 0;
}
