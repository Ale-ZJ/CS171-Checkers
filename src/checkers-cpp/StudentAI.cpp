/*** 
    Group Name: ArlongsHenchmen
    Group members:
        Sumpter, Danielle Lapre, dsumpter, 63383218
        Zhang Jiang, Alexandra, azhangji, 53188999
    Last Modified: 11/06/2022
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
    // make a new MCNode of the current board and add it into the MCTree
    Board copyBoard = board;
    MCNode headNode;
    headNode.board = copyBoard;
    headNode.parentNode = -1;
    MCTree.insert(headNode);

    // get the possible moves from the current board

    // make each move on a new board, then make a new MCNode for each new board,
    // adding each into the MCTree

    // do 20 simulations of the game

    // calculate the w_i/s_i rate for each possible move from the current board

    // find the move that results in the highest w_i/s_i

    // RETURN the highest w_i/s_i move
}


// NOTE: need to keep track of player
void StudentAI::simulateGames(Board board, MCNode curr)
{
    // if (current node is a leaf node)
        // if s_i != 0
            // curr node = expand(board)

        // new board = copy of board
        // int = rollout(new board, player)
        // backpropagation(int, current node)

        // RETURN

    // else (if not a leaf node)
        // child node = select()
        // RETURN simulateGames(child node)
}


Board StudentAI::select(MCNode curr)
{
    // keep track of the board with the highest UCT
    Board highestBoard;
    double highestUCT;

    // calculate the UCT of the children nodes
    for (unsigned int c : curr.children)
    {
        // find the child node with the highest UCT
        double u = calculateUCT(c);
        
        if (u > highestUCT)
        {
            highestUCT = u;
            highestBoard = MCTree[c].board;
        }
    }

    // RETURN the node with the highest UCT
    return highestBoard;
}


Board StudentAI::expand(Board b)
{
    // get all possible moves from the current board

    // make each move on a new board, then make a new MCNode for each new board,
    // adding each into the MCTree
        // save the first new MCNode

    // RETURN the first new child (aka first new MCNode)
}


// NOTE: need to change the turn for each recursive call of the rollout
int StudentAI::rollout(Board b, int turn)
{
    // if player 1, or 2 wins, or if there is a tie, then
    if (b.isWin(turn) == 1 | b.isWin(turn) == 2 | b.isWin(turn) == -1)
    {
        // if win for us/tie, return 1
        if (turn == player && (b.isWin(turn) == turn | b.isWin(turn) == -1))
        {
            return 1;
        }
        // else if loss for us, return 0
        else
        {
            return 0;
        }
    }

    // if the game board is terminal
    if (b.isWin(turn) != 0) {
        
    }

    // get all moves for the current board
    vector<vector<Move>> moves = b.getAllPossibleMoves(turn);

    // select and make a random move
    int i = rand() % (moves.size());
    vector<Move> checker_moves = moves[i];
    int j = rand() % (checker_moves.size());
    Move choice = checker_moves[j];
    b.makeMove(choice, turn);

    return rollout(b, (turn == 1) ? 2 : 1);
}


void StudentAI::backpropagate(int w, int nodeIdx)
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
        return backpropagate(w==1?0:1, MCTree[MCTree[nodeIdx].parentNode]);
    }
}


double StudentAI::calculateUCT(int nodeIdx)
{
    // get values for current child node w_i and s_i
    double w = MCTree[nodeIdx].w_i;
    double s = MCTree[nodeIdx].s_i;

    // access parent node and get s_p
    int parent = MCTree[nodeIdx].parentNode;
    double p = MCTree[parent].s_i;

    return (w/s) + (MCTS_UCT_CONS * sqrt(log(p) / s));
}



// ---------------- MINIMAX ALGORITHM  ------------------


// returns the difference in number of pieces
int StudentAI::evaluate(Board board)
{
    if (player == 2) return board.whiteCount - board.blackCount;
    else return board.blackCount - board.whiteCount;
}


Move StudentAI::minimax(int depth, Board board, int minimaxPlayer)
{
    return evalMax(depth, board, minimaxPlayer).move;
}


MinimaxPair StudentAI::evalMax(int depth, Board board, int maxPlayer)
{
    // check if the game terminates because of the opponent's move
    // in which case, there is no more move for us to make, OR
    // check if we have reached the desired recursive depth
    if(depth == 0 || board.isWin(1) || board.isWin(2))
    {
        return MinimaxPair{evaluate(board), Move()};
    }

    // keep track of the highest minimax evaluation value
    // max player takes the move with the highest value
    MinimaxPair maxValue_bestMove{numeric_limits<int>::min(), Move()};

    // getting all the possible moves for a given player
    vector<vector<Move>> moves = board.getAllPossibleMoves(maxPlayer);

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            Board new_board = board;
            new_board.makeMove(m, maxPlayer);
            MinimaxPair v2_m2 = evalMin(depth-1, new_board, maxPlayer == 2?1:2);

            if (v2_m2.value > maxValue_bestMove.value) 
            {
                maxValue_bestMove.value = v2_m2.value;
                maxValue_bestMove.move  = m;
            }
        }
    } 

    return maxValue_bestMove;
}


MinimaxPair StudentAI::evalMin(int depth, Board board, int minPlayer)
{
    // check if the game terminates because of the opponent's move
    // in which case, there is no more move for us to make, OR
    // check if we have reached the desired recursive depth
    if(depth == 0 || board.isWin(1) || board.isWin(2))
    {
        return MinimaxPair{evaluate(board), Move()};
    }

    // keep track of the highest minimax evaluation value
    // max player takes the move with the highest value
    MinimaxPair minValue_bestMove{numeric_limits<int>::max(), Move()};

    // getting all the possible moves for a given player
    vector<vector<Move>> moves = board.getAllPossibleMoves(minPlayer);

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            Board new_board = board;
            new_board.makeMove(m, minPlayer);
            MinimaxPair v2_m2 = evalMax(depth-1, new_board, minPlayer == 1?2:1);

            if (v2_m2.value < minValue_bestMove.value) 
            {
                minValue_bestMove.value = v2_m2.value;
                minValue_bestMove.move  = m;
            }
        }
    } 

    return minValue_bestMove;
}

    
