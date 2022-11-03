#include "StudentAI.h"
#include <random>

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

    // retrieve all the possible moves we can make, based on the current board state
    vector<vector<Move>> moves = board.getAllPossibleMoves(player);

//    randomly select a valid move to make
    //    int i = rand() % (moves.size());
    //    vector<Move> checker_moves = moves[i];
    //    int j = rand() % (checker_moves.size());
    //    Move res = checker_moves[j];

    


    // make the chosen move
    board.makeMove(res,player);

    // return the chosen move to the opponent
    return res;


}

int StudentAI::evaluation(Board board)
{
    // returns the difference in number of pieces
    return board.blackCount - board.whiteCount;
    // REMEMBER: add isKing difference for a better heuristic
}