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

    // retrieve all the possible moves that one player can make, 
    // based on the current board state
    // vector<vector<Move>> moves = board.getAllPossibleMoves(player);

    //    randomly select a valid move to make
    //    int i = rand() % (moves.size());
    //    vector<Move> checker_moves = moves[i];
    //    int j = rand() % (checker_moves.size());
    //    Move res = checker_moves[j];

    Move res = minimax(MINIMAX_DEPTH, board, 2); 

    // make the chosen move
    board.makeMove(res,2);

    // return the chosen move to the opponent
    return res;

}


// returns the difference in number of pieces
int StudentAI::evaluate(Board board)
{
    return board.blackCount - board.whiteCount;
    // REMEMBER: add isKing difference for a better heuristic
}


Move StudentAI::minimax(int depth, Board board, int minimaxPlayer)
{
    return evalMax(depth, board, minimaxPlayer).move;
}


MinimaxPair StudentAI::evalMax(int depth, Board board, int maxPlayer)
{
    //cout << "StudentAI::evalMax() : inside evalMax" << endl;
    // check if the game terminates because of the opponent's move
    // in which case, there is no more move for us to make, OR
    // check if we have reached the desired recursive depth
    if(depth == 0 || board.isWin(1))
    {
        //cout << "StudentAI::evalMax() : inside terminal check" << endl;
        return MinimaxPair{evaluate(board), Move()};
        
        // pair<int, Move> evalMove = {evaluate(board), nullptr};
        // return evalMove;
    }

    // keep track of the highest minimax evaluation value
    // max player takes the move with the highest value
    MinimaxPair maxValue_bestMove{numeric_limits<int>::min(), Move()};

    // pair<int, Move> maxValue_bestMove;
    // maxValue_bestMove.first = numeric_limits<int>::min();
    // maxValue_bestMove.second = nullptr;

    // getting all the possible moves for a given player
    vector<vector<Move>> moves = board.getAllPossibleMoves(2);
    //cout << "StudentAI::evalMax() : possible moves is " << moves.size() << endl;

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            //cout << "StudentAI::evalMax() : inside nested for loop" << endl;

            Board new_board = board;

            //cout << "StudentAI::evalMax() : new board below c:" << endl;
            //new_board.showBoard();


            new_board.makeMove(m, 2);

            //cout << "StudentAI::evalMax() : deep copy made" << endl;

            // pair<int, Move> v2_m2 = evalMin(depth-1, new_board, maxPlayer == 2 ? 1 : 2);
            MinimaxPair v2_m2 = evalMin(depth-1, new_board, 1);

            if (v2_m2.value > maxValue_bestMove.value) 
            {
                cout << "StudentAI::evalMax() : heuristic number is: " << v2_m2.value << endl;
                maxValue_bestMove.value = v2_m2.value;
                maxValue_bestMove.move  = m;
            }
        }
    } 

    return maxValue_bestMove;
}


MinimaxPair StudentAI::evalMin(int depth, Board board, int minPlayer)
{
    //cout << "StudentAI::evalMin() : inside evalMin" << endl;
    // check if the game terminates because of the opponent's move
    // in which case, there is no more move for us to make, OR
    // check if we have reached the desired recursive depth
    if(depth == 0 || board.isWin(2))
    {
        return MinimaxPair{evaluate(board), Move()};
    }

    // keep track of the highest minimax evaluation value
    // max player takes the move with the highest value
    MinimaxPair minValue_bestMove{numeric_limits<int>::max(), Move()};

    // pair<int, Move> minValue_bestMove;
    // minValue_bestMove.first = numeric_limits<int>::max();
    // minValue_bestMove.second = nullptr;

    // getting all the possible moves for a given player
    vector<vector<Move>> moves = board.getAllPossibleMoves(1);

    for (vector<Move> checker_moves : moves)
    {
        for (Move m : checker_moves)
        {
            Board new_board = board;
            new_board.makeMove(m, 1);

            // pair<int, Move> v2_m2 = evalMin(depth-1, new_board, minPlayer == 1 ? 2 : 1);
            MinimaxPair v2_m2 = evalMax(depth-1, new_board, 2);

            if (v2_m2.value < minValue_bestMove.value) 
            {
                cout << "StudentAI::evalMin() : heuristic number is: " << v2_m2.value << endl;
                minValue_bestMove.value = v2_m2.value;
                minValue_bestMove.move  = m;
            }
        }
    } 

    //return minValue_bestMove;
    return minValue_bestMove;
}

    
