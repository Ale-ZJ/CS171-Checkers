#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#include "Checker.h"
#include <vector>
#pragma once


// struct MinimaxPair{
//     int value; 
//     Move move;
// };


class MCNode
{
public:
    Board board;
    unsigned int turn;  // turn that got us here
    int score;
    unsigned int visits; 
    Move action;        // action that got us here
    MCNode* parent;
    vector<MCNode*> children;

private:
    const int MAX_ROLLOUTS = 40;
    const int MCTS_UCT_CONS = 2;

public:
    MCNode();
    MCNode(Board b, unsigned int t, Move a, MCNode* p);
    MCNode(const MCNode& other);


    // Increases the depth of the game tree by one layer
    MCNode* expand();

    // Randomly plays a game until either there is a winner or we have played
    // a set amount of turns, in which case, the winner is the one
    // with the highest heuristics
    int rollout();

    // Returns the children node with the highest UCT score
    MCNode* select();

    // Updates the score and visits of the node until we reach the head of the game tree
    void backpropagate(int winner);


    // Returns a heuristic/utility value given a state of the game
    double evaluate(Board b, int player);

    // Calculates the UCT value of a node. This value is used to balance exploitation 
    // and exploration of the game tree
    double calculateUCT();
};





//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
public: // given code DO NOT CHANGE
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);


private: 
    MCNode* root;
    

private: // member variables that we added
//     const int MINIMAX_DEPTH = 3;
    const int MATURITY_THRESHOLD = 200;
    const int NUMBER_OF_SIMULATIONS = 1100;



private: // Monte Carlo Tree Search algorithm
    Move mcts();

    // simulates an entire game following the monte carlo algorithm steps
    // selection, expansion, rollout, and backpropagation 
    void simulateGames();

//     // Selects the children node with the highest UCT value
//     int select(int nodeIdx);

//     // Increases the depth of the game tree by one level.
//     // We expand when we reach a leaf node in the MCTree that has 
//     // already been visited 
//     int expand(int parentIdx, int turn);

//     // Randomly plays a game until one player win.
//     // returns 1 if the rollout results in a win for the selectedPlayer
//     // or 0 if the player lost
//     double rollout(int nodeIdx, int turn);

//     // updates the w_i and s_i member variables from a given node 
//     // PRE-CONDITION: check the player's turn to pass the correct win value
//     void backpropagate(double score, int nodeIdx);

//     // calculates the UCT value of the node at the given index.
//     // UCT value = (w_i/s_i) + (MCTS_UCT * sqrt(log(s_p)/s_i))
//     // The AI will select the move with the highest UCT value
//     double calculateUCT(int nodeIdx);

//     //MCNode makeNewMCNode(Board b, Move parentMove, int parentIdx);

//     // given a board state and a player's turn, we calculate all the possible moves 
//     // for the player and we add them to the tree. We also update the parentNode a needed
//     void addMovesToTree(int parentIdx, int turn);

// private: // minimax algorithm
//     // returns the utility/heuristic value given the state of the game
//     double evaluate(const Board& board, int selectedPlayer);

    // // recursive algorithm to pick best move
    // int minimax(Board board, int minimaxPlayer, int depth);

    // //pair<int, Move> evalMin(int depth, Board board, int maxPlayer);
    // int evalMax(Board board, int maxPlayer, int selectedPlayer, int depth, int alpha, int beta);
    // int evalMin(Board board, int minPlayer, int selectedPlayer, int depth, int alpha, int beta);
};

#endif //STUDENTAI_H
