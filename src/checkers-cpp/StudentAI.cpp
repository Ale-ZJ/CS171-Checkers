/*** 
    Group Name: ArlongsHenchmen
    Group members:
        Sumpter, Danielle Lapre, dsumpter, 63383218
        Zhang Jiang, Alexandra, azhangji, 53188999
    Last Modified: 11/17/2022

    Notes:
        - if there is a seg fault, change the indexing from [] to .at() !!!
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

    // Move res = minimax(MINIMAX_DEPTH, board, player); 
    Move res = mcts();

    // make the chosen move
    board.makeMove(res, player);

    // return the chosen move to the opponent
    return res;

}


// -------- MONTE CARLO SEARCH TREE ALGORITHM --------------

// NOTE: make function to create new MCNode and add it to the MCTree

Move StudentAI::mcts()
{
    // make a new MCNode of the current board
    MCNode headNode = makeNewMCNode(board, Move(), -1);
    // Board copyBoard = board;
    // MCNode headNode;
    // headNode.board = copyBoard;
    // headNode.w_i = 0;
    // headNode.s_i = 0;
    // headNode.parentMove = Move();
    // headNode.parentNode = -1;
    // headNode.children = {};

    // add the head node to the MCTree
    MCTree.push_back(headNode);

    cout << "mcts(): initial tree size is " << MCTree.size() << endl;

    // get the possible moves from the current board AND make each move on a
    // new board. then make a new MCNode for each new board, adding each into
    // the MCTree.
    expand(board, 0, player);

    cout << "mcts(): after expansion tree size is " << MCTree.size() << endl;
    cout << "mcts(): head node childrens is " << MCTree.at(0).children.size() << endl;

    // if (MCTree.size() == 0) cout << "mcts(): no node added :((" << endl;
    // else cout << "mcts(): there is something :o" << endl;

    // do 20 simulations of the game
    for (int i = 0; i < 20; i++)
    {
        // NOTE: will always simulate on the head node and begin with our turn 
        simulateGames(board, 0, player);
        cout << "mcts(): " << i << "th game simulation successfull!" << endl;
    }

    // calculate the w_i/s_i rate for each possible move from the current board
    Move bestMove = Move();
    double highestRate = 0;

    // for (unsigned int n : headNode.children)
    for (unsigned int n : MCTree.at(0).children)
    {
        double rate = MCTree.at(n).w_i / MCTree.at(n).s_i;

        // NOTE: if two nodes have the same rate, prefer the first one
        if (rate > highestRate)
        {
            highestRate = rate;
            bestMove = MCTree.at(n).parentMove;
        }
    }

    // RETURN the highest w_i/s_i move
    return bestMove;
}


// NOTE: need to keep track of player
void StudentAI::simulateGames(Board b, int nodeIdx, int turn)
{
    cout << "simulateGames(): in" << endl;

    // if (current node is a leaf node)
    if (MCTree.at(nodeIdx).children.size() == 0)
    {
        cout << "simulateGames(): inside if, current node is leaf" << endl;

        // if the node has not been visited
        if (MCTree.at(nodeIdx).s_i != 0)
        {
            nodeIdx = expand(b, nodeIdx, turn);
            
            cout << "simulateGames(): exp successful" << endl;
        }

        // new board = copy of board
        Board new_board = b;

        // int = rollout(new board, player)
        int w = rollout(new_board, turn, turn);
        cout << "simulateGames(): rollout successful" << endl;

        // backpropagation(int, current node)
        backpropagate(w, nodeIdx);
        cout << "simulateGames(): backpropagate successful" << endl;

        // RETURN
        return;
    }
    // else (if not a leaf node)
    else
    {
        cout << "simulateGames(): inside else" << endl;

        // child node = select()
        int childIdx = select(nodeIdx);
        
        cout << "simulateGames(): select successful" << endl;

        // RETURN simulateGames(child node)
        return simulateGames(b, childIdx, turn == 1?2:1);
    }
}


int StudentAI::select(int nodeIdx)
{
    cout << "select(): in" << endl;

    // keep track of the board with the highest UCT
    // default favorite child is the first one
    int favoriteChild = MCTree.at(nodeIdx).children.at(0);
    double highestUCT = 0;

    // calculate the UCT of the children nodes
    for (unsigned int c : MCTree.at(nodeIdx).children)
    {
        // find the child node with the highest UCT
        double u = calculateUCT(c);
        cout << "select() child#" << c << " UCT is " << u << endl;
        
        if (u > highestUCT)
        {
            highestUCT = u;
            favoriteChild = c;
        }
    }

    cout << "select(): favoriteChild is " << favoriteChild << endl;

    // RETURN the node with the highest UCT
    return favoriteChild;
}


int StudentAI::expand(Board b, int parentIdx, int turn)
{
    // make each move on a new board, then make a new MCNode for each new board,
    // adding each into the MCTree
    addMovesToTree(b, parentIdx, turn);

    return MCTree.at(parentIdx).children.at(0);
}


// NOTE: need to change the turn for each recursive call of the rollout
int StudentAI::rollout(Board b, int turn, int selectedPlayer)
{
    // if the game board is terminal
    if (b.isWin(turn) != 0) {
        // if the selected node wins or ties then return 1
        if (b.isWin(selectedPlayer) == selectedPlayer | b.isWin(selectedPlayer) == -1) return 1;
        // else the selected node lost
        else return 0;
    }

    // get all moves for the current board
    vector<vector<Move>> moves = b.getAllPossibleMoves(turn);

    // select and make a random move
    int i = rand() % (moves.size());
    vector<Move> checker_moves = moves[i];
    int j = rand() % (checker_moves.size());
    Move choice = checker_moves[j];
    b.makeMove(choice, turn);

    return rollout(b, turn == 1?2:1, selectedPlayer);
}


void StudentAI::backpropagate(int w, int nodeIdx)
{
    MCTree.at(nodeIdx).w_i += w;
    MCTree.at(nodeIdx).s_i += 1;

    // if no parent, the node is the head
    if (MCTree.at(nodeIdx).parentNode == -1)
    {
        return;
    }
    else
    {
        int parentIdx = MCTree.at(nodeIdx).parentNode;
        return backpropagate(w==1?0:1, parentIdx);
    }
}


double StudentAI::calculateUCT(int nodeIdx)
{
    // get values for current child node w_i and s_i
    double w = MCTree.at(nodeIdx).w_i;
    double s = MCTree.at(nodeIdx).s_i;
    cout << "w, s pair is " << w << " / " << s << endl;

    // access parent node and get s_p
    int parent = MCTree.at(nodeIdx).parentNode;
    double p = MCTree.at(parent).s_i;
    cout << "s_p value is " << p << endl;

    double w_div_s = double(w) / s;
    cout << "w / s is " << w_div_s << endl;

    double l_div_s = log(p) / s;
    cout << "log(s_p) / s is " << l_div_s << endl;

    return w_div_s + (MCTS_UCT_CONS * sqrt(l_div_s));
}


// NEW
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


// // returns the difference in number of pieces
// int StudentAI::evaluate(Board board)
// {
//     if (player == 2) return board.whiteCount - board.blackCount;
//     else return board.blackCount - board.whiteCount;
// }


// Move StudentAI::minimax(int depth, Board board, int minimaxPlayer)
// {
//     return evalMax(depth, board, minimaxPlayer).move;
// }


// MinimaxPair StudentAI::evalMax(int depth, Board board, int maxPlayer)
// {
//     // check if the game terminates because of the opponent's move
//     // in which case, there is no more move for us to make, OR
//     // check if we have reached the desired recursive depth
//     if(depth == 0 || board.isWin(1) || board.isWin(2))
//     {
//         return MinimaxPair{evaluate(board), Move()};
//     }

//     // keep track of the highest minimax evaluation value
//     // max player takes the move with the highest value
//     MinimaxPair maxValue_bestMove{numeric_limits<int>::min(), Move()};

//     // getting all the possible moves for a given player
//     vector<vector<Move>> moves = board.getAllPossibleMoves(maxPlayer);

//     for (vector<Move> checker_moves : moves)
//     {
//         for (Move m : checker_moves)
//         {
//             Board new_board = board;
//             new_board.makeMove(m, maxPlayer);
//             MinimaxPair v2_m2 = evalMin(depth-1, new_board, maxPlayer == 2?1:2);

//             if (v2_m2.value > maxValue_bestMove.value) 
//             {
//                 maxValue_bestMove.value = v2_m2.value;
//                 maxValue_bestMove.move  = m;
//             }
//         }
//     } 

//     return maxValue_bestMove;
// }


// MinimaxPair StudentAI::evalMin(int depth, Board board, int minPlayer)
// {
//     // check if the game terminates because of the opponent's move
//     // in which case, there is no more move for us to make, OR
//     // check if we have reached the desired recursive depth
//     if(depth == 0 || board.isWin(1) || board.isWin(2))
//     {
//         return MinimaxPair{evaluate(board), Move()};
//     }

//     // keep track of the highest minimax evaluation value
//     // max player takes the move with the highest value
//     MinimaxPair minValue_bestMove{numeric_limits<int>::max(), Move()};

//     // getting all the possible moves for a given player
//     vector<vector<Move>> moves = board.getAllPossibleMoves(minPlayer);

//     for (vector<Move> checker_moves : moves)
//     {
//         for (Move m : checker_moves)
//         {
//             Board new_board = board;
//             new_board.makeMove(m, minPlayer);
//             MinimaxPair v2_m2 = evalMax(depth-1, new_board, minPlayer == 1?2:1);

//             if (v2_m2.value < minValue_bestMove.value) 
//             {
//                 minValue_bestMove.value = v2_m2.value;
//                 minValue_bestMove.move  = m;
//             }
//         }
//     } 

//     return minValue_bestMove;
// }

    
