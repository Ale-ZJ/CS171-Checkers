#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#include <vector>
#pragma once


struct MinimaxPair{
    int value; 
    Move move;
};


struct MCNode{
    Board board;
    unsigned int w_i;
    unsigned int s_i;
    Move parentMove;

    // we will keep track of the parent and child nodes by
    // an index in a vector that holds all nodes
    int parentNode; // -1 if the node is the head
    vector<unsigned int> children;
};


//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
public: // given code DO NOT CHANGE
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);


private: // member variables that we added
    const int MINIMAX_DEPTH = 3;
    vector<MCNode> MCTree;


private: // Monte Carlo Tree Search algorithm
    Move mcts();
    void simulateGames(Board board, MCNode curr);
    Board select(MCNode curr);
    Board expand(Board b);
    int rollout(Board b);
    void backpropagate(int w, MCNode curr);
    double calculateUCT(int nodeIdx);


private: // minimax algorithm
    // returns the utility/heuristic value given the state of the game
    int evaluate(Board board);

    // recursive algorithm to pick best move
    Move minimax(int depth, Board board, int minimaxPlayer);

    //pair<int, Move> evalMin(int depth, Board board, int maxPlayer);
    MinimaxPair evalMax(int depth, Board board, int maxPlayer);
    MinimaxPair evalMin(int depth, Board board, int minPlayer);
};

#endif //STUDENTAI_H
