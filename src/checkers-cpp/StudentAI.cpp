/*** 
    Group Name: ArlongsHenchmen
    Group members:
        Sumpter, Danielle Lapre, dsumpter, 63383218
        Zhang Jiang, Alexandra, azhangji, 53188999
    Last Modified: 12/04/2022
***/


#include "StudentAI.h"
#include <random>
#include <limits>
#include <iostream>
#include <cmath>

using namespace std;



//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col,int row,int p)
	:AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
}


Move StudentAI::GetMove(Move move)
{
    // add the opponent's move to our board
    if (move.seq.empty())
    {
        player = 1;
    } else{
        board.makeMove(move, player == 1?2:1);
    }

    Move res = mcts();
    board.makeMove(res, player);

    return res;
}


// ------------------- MCNode -----------------------

MCNode::MCNode()
    : visits(1)
{
}


MCNode::MCNode(Board b, unsigned int t, Move a, MCNode* p)
    : board(b), turn(t), score(0), visits(1), action(a), parent(p), children({})
{
}


MCNode::MCNode(const MCNode& other)
    : board(other.board), turn(other.turn), score(other.score), visits(other.visits), 
      action(other.action), parent(other.parent), children(other.children)
{
}


MCNode* MCNode::expand()
{
    vector<vector<Move>> moves = this->board.getAllPossibleMoves(3 - this->turn);

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            Board new_board = this->board;
            new_board.makeMove(m, 3 - this->turn);
            MCNode* child = new MCNode(new_board, 3 - this->turn, m, this);

            this->children.push_back( child );
        }
    }

    return (this->children.size() == 0) ? nullptr : this->children.at(0);
}


int MCNode::rollout()
{
    Board copyBoard = this->board;
    int nextTurn = 3 - this->turn;

    for (unsigned int r = 0; r < MAX_ROLLOUTS; ++r)
    {
        vector<vector<Move>> moves = copyBoard.getAllPossibleMoves(nextTurn);

        // no moves left for nextTurn, then winner is the current turn
        if(moves.size() == 0)
        {
            return 3 - nextTurn;
        }

        // random moves
        int i = rand() % (moves.size());
        vector<Move> checker_moves = moves[i];
        int j = rand() % (checker_moves.size());
        Move choice = checker_moves[j];

        copyBoard.makeMove(choice, nextTurn);

        // switch turn
        nextTurn = 3 - nextTurn;
    }

    // the game is not terminal yet, then we evaluate the current board
    // and the player with the highest evaluation score is the winner
    double p1 = evaluate(copyBoard, this->turn);
    double p2 = evaluate(copyBoard, 3 - this->turn);

    if (p1 > p2) return this->turn;             // we win
    else if (p1 < p2) return 3 - this->turn;    // we lose
    else return 0;                              // tie
}


MCNode* MCNode::select()
{
    // default favorite child is the first one
    double highestUCT = this->children.at(0)->calculateUCT();
    MCNode* favChild = this->children.at(0);
 
    // there are more children to select from
    for (unsigned int i = 1; i < this->children.size(); ++i)
    {
        double u = this->children.at(i)->calculateUCT();

        if (u > highestUCT)
        {
            highestUCT = u;
            favChild = this->children.at(i);
        }
    }

    return favChild;
}


void MCNode::backpropagate(int winner)
{
    MCNode* curr = this;

    while(curr != nullptr)
    {
        curr->visits++;

        // alternate how we update the score
        if (winner == curr->turn)
        {
            curr->score += 1;
        }
        else if (winner == (3 - curr->turn))
        {
            curr->score -= 1;
        }

        curr = curr->parent;
    }
}



double MCNode::evaluate(Board b, int player)
{
    double score = 0.0;
    char playerColor = (player == 1) ? 'B' : 'W';

    for (vector<Checker> row : b.board)
    {
        for (Checker c : row)
        {
            if (c.color[0] == playerColor && c.isKing) score += 0.5;
        }
    }

    if (player == 2) score += (b.whiteCount - b.blackCount);
    else score += (b.blackCount - b.whiteCount);

    return score;
}


double MCNode::calculateUCT()
{
    double w_s = (double) this->score / (double) this->visits;
    double l_s = log(this->parent->visits) / (double) this->visits;
    
    return w_s + (MCTS_UCT_CONS * sqrt(l_s));
}




// -------- MONTE CARLO SEARCH TREE ALGORITHM --------------


Move StudentAI::mcts()
{
    // adding the head node to the tree
    root = new MCNode(board, 3 - player, Move(), nullptr);

    // initial expansion
    root->expand();
    
    // run number of game simulations
    for (unsigned int i = 0; i < NUMBER_OF_SIMULATIONS; ++i)
    {
        simulateGames();
    }

    // find the bestChild
    MCNode* bestChild = root->children.at(0);
    double mostVisits = root->children.at(0)->visits;

    //cout << "mcts(): root score/visits = " << root->score << "/" << root->visits << endl;
    //cout << "mcts(): child#0 move " << root->children.at(0)->action.toString() << " score/visits = " << root->children.at(0)->score << "/" << root->children.at(0)->visits << " and UCT " << highestUCT << endl;

    for (unsigned int i = 1; i < root->children.size(); ++i)
    {
        double v = root->children.at(i)->visits;

        //cout << "mcts(): child#" << i << " move " << root->children.at(i)->action.toString() << " score/visits = " << root->children.at(i)->score << "/" << root->children.at(i)->visits << " and UCT " << u << endl;

        if (v > mostVisits)
        {
            //cout << "mcts(): found fav child = " << i << " with UCT " << u << endl;
            bestChild = root->children.at(i);
            mostVisits = v;
        }
    }

    return bestChild->action;
}


void StudentAI::simulateGames()
{
    MCNode* node = root;
    int winner = 0;

    while(true)
    {
        if (node->children.size() == 0)
        {
            if (node->visits >= MATURITY_THRESHOLD)
            {
                MCNode* expandedNode = node->expand();

                // terminal node - go directly to backpropagation
                if (expandedNode == nullptr)
                {
                    winner = node->turn;
                    break;
                }

                node = expandedNode;
                continue;
            }

            winner = node->rollout();
            break;
        }

        // select node with highest UCT
        node = node->select();
    }

    // backpropagate the winner and update number of simulation
    node->backpropagate(winner);
}

