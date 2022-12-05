/*** 
    Group Name: ArlongsHenchmen
    Group members:
        Sumpter, Danielle Lapre, dsumpter, 63383218
        Zhang Jiang, Alexandra, azhangji, 53188999
    Last Modified: 12/02/2022
***/


#include "StudentAI.h"
#include <random>
#include <limits>
#include <iostream>
#include <cmath>

using namespace std;




// MCNode::MCNode()
// {
// }


// MCNode::MCNode(Board b, Move a, int p)
//     : board(b), score(0), visits(0), action(a), parent(p), children({})
// {
// }


// MCNode::MCNode(const MCNode& other)
// {
//     board       = other.board;
//     score       = other.score;
//     visits      = other.visits;
//     action      = other.action;
//     parent      = other.parent;

//     for (unsigned int c : other.children)
//     {
//         children.push_back(c);
//     }
// }





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


// ------------------- HELPERS -----------------------

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



































































// Move StudentAI::mcts()
// {
//     // make a new MCNode of the current board and add to MCTree
//     Board copyBoard = board;
//     MCNode headNode = MCNode(copyBoard, Move(), -1);
//     MCTree.push_back(headNode);

//     // initial expansion adds all possible moves for player
//     addMovesToTree(0, player);

//     // conduct simulations of game
//     for (int i = 0; i < NUMBER_OF_SIMULATIONS; i++)
//     {
//         // NOTE: will always simulate on the head node and begin with our turn 
//         simulateGames(0, 3 - player);
//     }

//     // select the immediate children who has the highest score
//     unsigned int selectedChild = 1;
//     double highestScore = calculateUCT(1);
    
//     for (unsigned int c = 2; c <= MCTree[0].children.size(); ++c)
//     {     
//         double u = calculateUCT(c);

//         // cout << "child #" << c << " uct " << u << endl;   
//         // NOTE: if two nodes have the same # score, prefer the last one
//         if (u >= highestScore)
//         {
//             //cout << "found new fav child with score " << MCTree[n].score << endl;
//             highestScore = u;
//             selectedChild = c;
//         } 
//    }

//     cout << "mcts(): child " << selectedChild << " move " << MCTree[selectedChild].action.toString() << endl;

//     return MCTree[selectedChild].action;
// }


// // NOTE: need to keep track of turn
// void StudentAI::simulateGames(int nodeIdx, int turn)
// {
//     // traverse until you get a leaf node
//     while (MCTree[nodeIdx].children.size() != 0)
//     {
//         nodeIdx = select(nodeIdx);
//         // cout << "simulateGames() select ok" << endl;
//         turn = 3 - turn;
//     }

//     // if the leaf node is terminal then return
//     if ( MCTree[nodeIdx].board.isWin(turn) != 0 )
//     {
//         return;
//     }

//     // if a "mature" node, then expand and retrieve the corresponding board
//     // for that new child node

//     // [NOTE:] a "mature" node is one that has been simulated over a given amount
//     // of times :)
//     if (MCTree[nodeIdx].visits == MATURITY_THRESHOLD)
//     {
//         // cout << "simulateGames() mature node" << endl;
//         nodeIdx = expand(nodeIdx, turn);
//         // cout << "simulateGames() expand ok" << endl;
//         turn = 3 - turn;
//     }

//     double score = rollout(nodeIdx, turn);
//     // cout << "simulateGames() rollout ok" << endl;

//     backpropagate(score, nodeIdx);
//     // cout << "simulateGames() backpropagate ok" << endl;
// }


// int StudentAI::select(int nodeIdx)
// {
//     // keep track of the board with the highest UCT
//     // default favorite child is the first one
//     int favoriteChild = MCTree[nodeIdx].children[0];
//     double highestUCT = 0;

//     // calculate the UCT of the children nodes
//     for (unsigned int c : MCTree[nodeIdx].children)
//     {
//         double u = calculateUCT(c);
        
//         if (u > highestUCT)
//         {
//             highestUCT = u;
//             favoriteChild = c;
//         }
//     }

//     return favoriteChild;
// }


// int StudentAI::expand(int parentIdx, int turn)
// {
//     // add all possible moves from the current board state to the MCTree
//     addMovesToTree(parentIdx, turn);

//     // return the first child move seen
//     return MCTree[parentIdx].children[0];
// }


// double StudentAI::rollout(int nodeIdx, int turn)
// {
//     Board copyBoard = MCTree[nodeIdx].board;

//     // rollout until a certain depth
//     for (unsigned int r = 0; r < MAX_ROLLOUTS; ++r)
//     {
//         vector<vector<Move>> moves = copyBoard.getAllPossibleMoves(turn);

//         // no moves left, the game is terminal
//         if (moves.size() == 0)
//         {
//             return evaluate(copyBoard, player);
//         }
        
//         // select and make a random move
//         int i = rand() % (moves.size());
//         vector<Move> checker_moves = moves[i];
//         int j = rand() % (checker_moves.size());
//         Move choice = checker_moves[j];

//         copyBoard.makeMove(choice, turn);
//         turn = 3 - turn;    
//     }

//     // if the board is not terminal, then call evaluate function
//     // cout << "rollout(): finished for node #" << nodeIdx << " with score " << evaluate(copyBoard, player) << endl;
//     return evaluate(copyBoard, player);
// }


// void StudentAI::backpropagate(double score, int nodeIdx)
// {
//     while(nodeIdx != -1)
//     {
//         MCTree[nodeIdx].score  += score;
//         MCTree[nodeIdx].visits += 1;

//         nodeIdx = MCTree[nodeIdx].parent;
//     }
// }


// double StudentAI::calculateUCT(int nodeIdx)
// {
//     // get values for current child node w_i and s_i
//     double w_i = MCTree[nodeIdx].score;
//     double s_i = MCTree[nodeIdx].visits;
//     // cout << "w, s pair is " << w_i << " / " << s_i << endl;

//     // access parent node and get s_p
//     int parent = MCTree[nodeIdx].parent;
//     double s_p = MCTree[parent].visits;
//     // cout << "s_p value is " << s_p << endl;

//     double w_div_s;
//     double l_div_s;

//     if (s_i == 0)
//     {
//         // division by 0 is +infinity
//         w_div_s = numeric_limits<double>::max();
//         l_div_s = numeric_limits<double>::max();
//     }
//     else
//     {
//         w_div_s = w_i / s_i;
//         l_div_s = log(s_p) / s_i;
//     }
//     //cout << "w / s is " << w_div_s << endl;
//     //cout << "log(s_p) / s is " << l_div_s << endl;

//     return w_div_s + (MCTS_UCT_CONS * sqrt(l_div_s));
// }


// void StudentAI::addMovesToTree(int parentIdx, int turn)
// {
//     Board b = MCTree[parentIdx].board;
//     vector<vector<Move>> moves = b.getAllPossibleMoves(turn);

//     for (vector<Move> checker_moves : moves)
//     {
//         for (Move m : checker_moves)
//         {
//             Board newBoard = b;
//             newBoard.makeMove(m, turn);

//             // initialize new child and add to MCTree
//             MCNode child = MCNode(newBoard, m, parentIdx);
//             MCTree.push_back(child);

//             // insert children index
//             // the children is the last added node into the vector, hence vector.size -1
//             MCTree[parentIdx].children.push_back(MCTree.size() - 1); 
//         }
//     }
// }



// // ---------------- MINIMAX ALGORITHM  ------------------


// double StudentAI::evaluate(const Board& b, int selectedPlayer)
// {
//     double score = 0.0;
//     char playerColor = (selectedPlayer == 1) ? 'B' : 'W';

//     for (vector<Checker> row : b.board)
//     {
//         for (Checker c : row)
//         {
//             if (c.color[0] == playerColor && c.isKing) score += 0.5;
//         }
//     }

//     if (selectedPlayer == 2) score += (b.whiteCount - b.blackCount);
//     else score += (b.blackCount - b.whiteCount);

//     return score;
// }


// int StudentAI::minimax(Board board, int minimaxPlayer, int depth)
// {
//     int heuristic_val = evalMax(board, minimaxPlayer, minimaxPlayer, depth, numeric_limits<int>::min(), numeric_limits<int>::max());

//     if (heuristic_val <= 0) return 0;
//     else return 1;
// }


// int StudentAI::evalMax(Board board, int maxPlayer, int selectedPlayer, int depth, int alpha, int beta)
// {
//     // check if the game terminates because of the opponent's move
//     // in which case, there is no more move for us to make, OR
//     // check if we have reached the desired recursive depth
//     // if(depth == 0 || board.isWin(1) || board.isWin(2))
//     // {
//     //     return MinimaxPair{evaluate(board), Move()};
//     // }
//     if (board.isWin(selectedPlayer) != 0 || depth == 0) return evaluate(board);

//     // keep track of the highest minimax evaluation value
//     // max player takes the move with the highest value
//     int best = numeric_limits<int>::min();

//     // getting all the possible moves for a given player
//     vector<vector<Move>> moves = board.getAllPossibleMoves(maxPlayer);

//     for (int i = 0; i < moves.size(); i++)
//     {
//         for (int j = 0; j < moves[i].size(); j++)
//         {
//             Board new_board = board;
//             new_board.makeMove(moves[i][j], maxPlayer);
//             int val = evalMin(new_board, maxPlayer == 2?1:2, selectedPlayer, depth-1, alpha, beta);

//             // alpha-beta
//             best = max(best, val);
//             alpha = max(alpha, best);

//             // if PRUNING
//             if (beta <= alpha)
//             {
//                 return best;
//             }
//         }
//     }

//     return best;
// }


// int StudentAI::evalMin(Board board, int minPlayer, int selectedPlayer, int depth, int alpha, int beta)
// {
//     // check if the game terminates because of the opponent's move
//     // in which case, there is no more move for us to make, OR
//     // check if we have reached the desired recursive depth
//     // if(depth == 0 || board.isWin(1) || board.isWin(2))
//     // {
//     //     return MinimaxPair{evaluate(board), Move()};
//     // }
//     if (board.isWin(selectedPlayer) != 0 || depth == 0) return evaluate(board);

//     // keep track of the highest minimax evaluation value
//     // max player takes the move with the highest value
//     int best = numeric_limits<int>::max();

//     // getting all the possible moves for a given player
//     vector<vector<Move>> moves = board.getAllPossibleMoves(minPlayer);

//     for (int i = 0; i < moves.size(); i++)
//     {
//         for (int j = 0; j < moves[i].size(); j++)
//         {
//             Board new_board = board;
//             new_board.makeMove(moves[i][j], minPlayer);
//             int val = evalMax(new_board, minPlayer == 1?2:1, selectedPlayer, depth-1, alpha, beta);

//             // alpha-beta
//             best = min(best, val);
//             alpha = min(beta, best);

//             // if PRUNING
//             if (beta <= alpha)
//             {
//                 return best;
//             }
//         }
//     }

//     return best;
// }

    
