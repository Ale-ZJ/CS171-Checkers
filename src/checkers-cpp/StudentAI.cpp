/*** 
    Group Name: ArlongsHenchmen
    Group members:
        Sumpter, Danielle Lapre, dsumpter, 63383218
        Zhang Jiang, Alexandra, azhangji, 53188999
    Last Modified: 11/17/2022
***/


#include "StudentAI.h"
#include <random>
#include <limits>
#include <iostream>
#include <cmath>

using namespace std;


MCNode::MCNode()
{
}

// copy constructor
MCNode::MCNode(const MCNode& other)
{
    board = other.board;
    w_i = other.w_i;
    s_i = other.s_i;
    parentMove = other.parentMove;
    parentNode = other.parentNode;
    children = other.children;
}


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

    // cout << "AI is player number " << player << endl;
    // Move res = minimax(MINIMAX_DEPTH, board, player); 
    MCTree = vector<MCNode>();
    Move res = mcts();

    // cout << "getMove(): hi 0" << endl;
    // make the chosen move
    board.makeMove(res, player);
    // cout << "getMove(): hi 1" << endl;

    // return the chosen move to the opponent
    return res;

}


// -------- MONTE CARLO SEARCH TREE ALGORITHM --------------

// NOTE: make function to create new MCNode and add it to the MCTree

Move StudentAI::mcts()
{
    // make a new MCNode of the current board
    MCNode headNode = makeNewMCNode(board, Move(), -1);

    // add the head node to the MCTree
    MCTree.push_back(headNode);

    // get the possible moves from the current board AND make each move on a
    // new board. then make a new MCNode for each new board, adding each into
    // the MCTree.
    expand(board, 0, player);

    // conduct simulations of game
    for (int i = 0; i < NUMBER_OF_SIMULATIONS; i++)
    {
        // NOTE: will always simulate on the head node and begin with our turn 
        simulateGames(0, player == 1?2:1);
    }

    // calculate the UCT for each possible move from the head board state
    int selectedChild = 0;
    unsigned int visits = 0;
    
    for (unsigned int n : MCTree[0].children)
    {        
        // NOTE: if two nodes have the same # visits, prefer the first one
        if (MCTree[n].s_i > visits)
        {
            visits = MCTree[n].s_i;
            selectedChild = n;
        } 
    }

    // RETURN the move with the highest UCT
    return MCTree[selectedChild].parentMove;
}


// NOTE: need to keep track of turn
void StudentAI::simulateGames(int nodeIdx, int turn)
{
    // if necessary, traverse until you get a leaf node
    while (MCTree[nodeIdx].children.size() != 0)
    {
        nodeIdx = select(nodeIdx);
        turn = (turn == 1)?2:1;
    }

    Board new_board = MCTree[nodeIdx].board;

    // if a "mature" node, then expand and retrieve the corresponding board
    // for that new child node

    // [NOTE:] a "mature" node is one that has been simulated over a given amount
    // of times :)
    if (MCTree[nodeIdx].s_i == 100)
    {
        nodeIdx = expand(new_board, nodeIdx, turn);
        new_board = MCTree[nodeIdx].board;
    }

    double w = rollout(new_board, turn, turn);
    backpropagate(w, nodeIdx);
}


int StudentAI::select(int nodeIdx)
{
    // keep track of the board with the highest UCT
    // default favorite child is the first one
    int favoriteChild = MCTree[nodeIdx].children[0];
    double highestUCT = 0;

    // calculate the UCT of the children nodes
    for (unsigned int c : MCTree[nodeIdx].children)
    {
        // find the child node with the highest UCT
        double u = calculateUCT(c);
        
        if (u > highestUCT)
        {
            highestUCT = u;
            favoriteChild = c;
        }
    }

    // RETURN the node with the highest UCT
    return favoriteChild;
}


int StudentAI::expand(Board b, int parentIdx, int turn)
{
    // add all moves from the current board state to the MCTree
    addMovesToTree(b, parentIdx, turn);

    // return the first child move seen
    return MCTree[parentIdx].children[0];
}


double StudentAI::rollout(Board b, int turn, int selectedPlayer)
{
    unsigned int numRollouts = 0;
    unsigned int win = 0;

    // conduct rollout until (1) terminal node is reached or (2) the rollout
    // threshold is met and a board evaluation is therefore conducted
    while (numRollouts < 50)
    {
        // get all moves for the current board
        vector<vector<Move>> moves = b.getAllPossibleMoves(turn);

        Move choice = Move();

        // RETURN a tie if tie
        if (b.isWin(selectedPlayer) == -1) return 0.5;

        if (moves.size() == 0)
        {
            // RETURN a loss if we have no more moves
            if (turn != selectedPlayer) return 1;
            else return 0;
        }
        else if (moves.size() == 1 && moves[0].size() == 1)
        {
            // make that move
            choice = moves[0][0];
        }
        else
        {
            // select and make a random move
            int i = rand() % (moves.size());
            vector<Move> checker_moves = moves[i];
            int j = rand() % (checker_moves.size());
            choice = checker_moves[j];
        }

        b.makeMove(choice, turn);

        turn = (turn == 1)?2:1;
        numRollouts++;
    }

    // the difference in pieces on the current board state; board evaluation
    int difference = evaluate(b, selectedPlayer);

    if (difference > 0) return 1;
    else if (difference < 0) return 0;
    else return 0.5;

}


void StudentAI::backpropagate(double w, int nodeIdx)
{
    MCTree[nodeIdx].w_i += w;
    MCTree[nodeIdx].s_i += 1;

    // if no parent, the node is the head
    if (MCTree[nodeIdx].parentNode == -1)
    {
        return;
    }
    else
    {
        int parentIdx = MCTree[nodeIdx].parentNode;

        // if a tie, continue to backpropagate a tie, if not, then backpropagate
        // the opposite value (win/loss)
        if (w == 0.5) return backpropagate(w, parentIdx);
        else return backpropagate(w==1?0:1, parentIdx);
    }
}


double StudentAI::calculateUCT(int nodeIdx)
{
    // get values for current child node w_i and s_i
    double w = MCTree[nodeIdx].w_i;
    double s = MCTree[nodeIdx].s_i;
    //cout << "w, s pair is " << w << " / " << s << endl;

    // access parent node and get s_p
    int parent = MCTree[nodeIdx].parentNode;
    double p = MCTree[parent].s_i;
    //cout << "s_p value is " << p << endl;

    double w_div_s;
    double l_div_s;

    if (s == 0)
    {
        // division by 0 is +infinity
        w_div_s = numeric_limits<double>::max();
        l_div_s = numeric_limits<double>::max();
    }
    else
    {
        w_div_s = double(w) / s;
        l_div_s = log(p) / s;
    }
    //cout << "w / s is " << w_div_s << endl;
    //cout << "log(s_p) / s is " << l_div_s << endl;

    return w_div_s + (MCTS_UCT_CONS * sqrt(l_div_s));
}



MCNode StudentAI::makeNewMCNode(Board b, Move parentMove, int parentIdx)
{
    Board copyBoard = board;
    MCNode newNode;

    newNode.board = copyBoard;
    newNode.w_i = 0;
    newNode.s_i = 0;
    newNode.parentMove = parentMove;
    newNode.parentNode = parentIdx;
    newNode.children = {};

    return newNode;
}


void StudentAI::addMovesToTree(Board b, int parentIdx, int turn)
{
    vector<vector<Move>> moves = b.getAllPossibleMoves(turn);

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            Board new_board = b;
            new_board.makeMove(m, turn);

            // initialize new MCNode
            MCNode child = makeNewMCNode(new_board, m, parentIdx);
            // MCNode child;
            // child.board = new_board;
            // child.w_i = 0;
            // child.s_i = 0;
            // child.parentNode = parentIdx;
            // child.parentMove = m;

            // insert the new child MCNode to the tree
            MCTree.push_back(child);

            // insert children index
            // the children is the last added node into the vector, hence vector.size
            MCTree[parentIdx].children.push_back(MCTree.size() - 1); 
        }
    }
}



// ---------------- MINIMAX ALGORITHM  ------------------


// returns the difference in number of pieces
int StudentAI::evaluate(Board board, int selectedPlayer)
{
    if (selectedPlayer == 2) return board.whiteCount - board.blackCount;
    else return board.blackCount - board.whiteCount;
}


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

    
