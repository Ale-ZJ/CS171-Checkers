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
    const int MCTS_UCT_CONS = 2;
    vector<MCNode> MCTree;


private: // Monte Carlo Tree Search algorithm
    Move mcts();

    // simulates an entire game following the monte carlo algorithm steps
    // selection, expansion, rollout, and backpropagation 
    void simulateGames(Board board, int nodeIdx, int turn);

    // Selects the children node with the highest UCT value
    int select(int nodeIdx);

    // Increases the depth of the game tree by one level.
    // We expand when we reach a leaf node in the MCTree that has 
    // already been visited 
    int expand(Board b, int turn);

    // Randomly plays a game until one player win.
    // returns 1 if the rollout results in a win for the selectedPlayer
    // or 0 if the player lost
    int rollout(Board b, int turn, int selectedPlayer);

    // updates the w_i and s_i member variables from a given node 
    // PRE-CONDITION: check the player's turn to pass the correct win value
    void backpropagate(int w, int nodeIdx);

    // calculates the UCT value of the node at the given index.
    // UCT value = (w_i/s_i) + (MCTS_UCT * sqrt(log(s_p)/s_i))
    // The AI will select the move with the highest UCT value
    double calculateUCT(int nodeIdx);

    // given a board state and a player's turn, we calculate all the possible moves 
    // for the player and we add them to the tree. We also update the parentNode a needed
    void addMovesToTree(int parentIdx, Board b, int turn)

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
