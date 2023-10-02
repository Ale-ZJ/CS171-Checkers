#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#include "Checker.h"
#include <vector>
#pragma once



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
    const int MATURITY_THRESHOLD = 200;
    const int NUMBER_OF_SIMULATIONS = 1500;


private: // Monte Carlo Tree Search algorithm
    Move mcts();

    // simulates an entire game following the monte carlo algorithm steps
    // selection, expansion, rollout, and backpropagation 
    void simulateGames();

};

#endif //STUDENTAI_H
