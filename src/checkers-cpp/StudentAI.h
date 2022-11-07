#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#pragma once


struct MinimaxPair{
    int value; 
    Move move;
};

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
private: 
    const int MINIMAX_DEPTH = 3;

public:
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);

private:
    // returns the utility/heuristic value given the state of the game
    int evaluate(Board board);

    // recursive algorithm to pick best move
    Move minimax(int depth, Board board, int minimaxPlayer);

    //pair<int, Move> evalMin(int depth, Board board, int maxPlayer);
    MinimaxPair evalMax(int depth, Board board, int maxPlayer);
    MinimaxPair evalMin(int depth, Board board, int minPlayer);
};

#endif //STUDENTAI_H
