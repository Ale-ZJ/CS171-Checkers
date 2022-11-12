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
        board.makeMove(move,player == 1?2:1);
    }

    // Move res = minimax(MINIMAX_DEPTH, board, player); 
    Move res = mcts();

    // make the chosen move
    board.makeMove(res,player);

    // return the chosen move to the opponent
    return res;

}


// -------- MONTE CARLO SEARCH TREE ALGORITHM --------------


Move mcts(){
    // create a new map

    // make a new MCNode of the current board and add it into the MCTree
    Board copyBoard = board;
    MCNode headNode;
    headNode.board = copyBoard;
    headNode.parentNode = -1;
    MCTree.insert(headNode);

    // get the possible moves from the current board

    // make each move on a new board, then make a new MCNode for each new board,
    // adding each into the MCTree

    // do 4 simulations of the game

    // calculate the w_i/s_i rate for each possible move from the current board

    // find the move that results in the highest w_i/s_i

    // RETURN the highest w_i/s_i move
}


void simulateGames(Board board, MCNode curr)
{
    // if (current node is a leaf node)
        // if s_i != 0
            // curr node = expand(board)

        // new board = copy of board
        // int = rollout(new board)
        // backpropagation(int, current node)

        // RETURN

    // else (if not a leaf node)
        // child node = select()
        // RETURN simulateGames(child node)
}


Board select(MCNode curr)
{
    // calculate the UCT of the children nodes

    // find the child node with the highest UCT

    // RETURN the node with the highest UCT
}


Board expand(Board b)
{
    // get all possible moves from the current board

    // make each move on a new board, then make a new MCNode for each new board,
    // adding each into the MCTree
        // save the first new MCNode

    // RETURN the first new child (aka first new MCNode)
}


int rollout(Board b)
{
    // if (node is terminal)
    //      if win/tie, RETURN 1
    //      else, RETURN 0

    // get all moves for the current board

    // select a random move

    // make selected move on the board

    // RETURN rollout(board)
}


void backpropagate(int w, MCNode curr)
{
    // update node's w_i += int

    // s_i++

    // if (node is head)
    //      RETURN
    // else
    //      parent node = get parent node
    //      RETURN backpropagate(int, parent node)

}


double calculateUCT(int nodeIdx)
{
    // get values for w_i, s_i and s_p

    // frac = w_i/s_i

    // num = ln(s_p)

    // sq = sqrt(ln(s_p) / s_i)

    // sum = frac + (c * sq)

    // RETURN sum

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

    
