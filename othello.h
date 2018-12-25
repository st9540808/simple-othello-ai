#ifndef OTHELLO_H
#define OTHELLO_H

#include <iostream>
#include <ostream>
#include "bitboard.h"

class Othello {
public:
    Othello();
    state get_square_state(square);
    void set_square_state(square, state);
    
    friend std::ostream& operator<<(std::ostream& os, const Othello& othello);
private:
    Bitboard boards[num_colors];
};

#endif