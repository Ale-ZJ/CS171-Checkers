#include "StudentAI.h"
#include <random>
#include <limits>

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

    // retrieve all the possible moves that one player can make, 
    // based on the current board state
    // vector<vector<Move>> moves = board.getAllPossibleMoves(player);

    //    randomly select a valid move to make
    //    int i = rand() % (moves.size());
    //    vector<Move> checker_moves = moves[i];
    //    int j = rand() % (checker_moves.size());
    //    Move res = checker_moves[j];

    Move res = minimax(MINIMAX_DEPTH, board, player); 

    // make the chosen move
    board.makeMove(res,player);

    // return the chosen move to the opponent
    return res;

}

Move StudentAI::minimax(int depth, Board board, int minimaxPlayer)
{
    return evalMax(depth, board, minimaxPlayer).second;
}

pair<int, Move> evalMax(int depth, Board board, int maxPlayer)
{
    // check if the game terminates because of the opponent's move
    // in which case, there is no more move for us to make, OR
    // check if we have reached the desired recursive depth
    if(depth == 0 || board.isWin(maxPlayer == 2 ? 1 : 2))
    {
        pair<int, Move> evalMove = {evaluate(board), nullptr};
        return evalMove;
    }

    // keep track of the highest minimax evaluation value
    // max player takes the move with the highest value
    pair<int, Move> maxValue_bestMove;
    maxValue_bestMove.first = numeric_limits<int>::min();
    maxValue_bestMove.second = nullptr;

    // getting all the possible moves for a given player
    vector<vector<Move>> moves = board.getAllPossibleMoves(maxPlayer);

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            Board new_board = board;
            new_board.makeMove(m, maxPlayer);

            pair<int, Move> v2_m2 = evalMin(depth-1, new_board, maxPlayer == 2 ? 1 : 2);

            if (v2_m2.first > maxValue_bestMove.first) 
            {
                maxValue_bestMove = v2_m2;
            }
        }
    } 

    return maxValue_bestMove;
}


pair<int, Move> evalMin(int depth, Board board, int minPlayer)
{
    // check if the game terminates because of the opponent's move
    // in which case, there is no more move for us to make, OR
    // check if we have reached the desired recursive depth
    if(depth == 0 || board.isWin(minPlayer == 1 ? 2 : 1))
    {
        pair<int, Move> evalMove = {evaluate(board), nullptr};
        return evalMove;
    }

    // keep track of the highest minimax evaluation value
    // max player takes the move with the highest value
    pair<int, Move> minValue_bestMove;
    minValue_bestMove.first = numeric_limits<int>::max();
    minValue_bestMove.second = nullptr;

    // getting all the possible moves for a given player
    vector<vector<Move>> moves = board.getAllPossibleMoves(minPlayer);

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            Board new_board = board;
            new_board.makeMove(m, minPlayer);

            pair<int, Move> v2_m2 = evalMin(depth-1, new_board, minPlayer == 1 ? 2 : 1);

            if (v2_m2.first < minValue_bestMove.first) 
            {
                minValue_bestMove = v2_m2;
            }
        }
    } 

    return minValue_bestMove;
}

    

int StudentAI::evaluate(Board board)
{
    // returns the difference in number of pieces
    return board.blackCount - board.whiteCount;
    // REMEMBER: add isKing difference for a better heuristic
}