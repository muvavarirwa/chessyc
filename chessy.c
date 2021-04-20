#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BOARD_SIZE 64
#define NAME_SIZE 10
#define NUM_SIDES 2
#define MAX_MOVES 10
#define BYTE_SIZE 8
#define NUM_PIECES 7
#define NUM_MOVES 8
#define TUPLE 2
#define NUM_PAWNS 8
#define EMPTY_FLAG -1

struct pos {
    int row;
    int col;
};
    
struct move {
    int rowChange;
    int colChange;
};

struct move pawnMoves[3]   = {1,1,1,-1,1,0};
struct move rookMoves[3]   = {1,0,0,1,0,-1};
struct move knightMoves[6] = {1,2,2,1,1,-2,-2,1,-1,-2,-1,2};
struct move bishopMoves[4] = {1,1,1,-1,-1,1,-1,-1};
struct move queenMoves[16] = {1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1,1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1};
struct move kingMoves[16]  = {1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1,1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1};
struct move emptyMoves[2]  = {0,0,0,0};

struct move *moves[NUM_PIECES] = {pawnMoves, rookMoves, knightMoves, bishopMoves, queenMoves, kingMoves, emptyMoves}; 

//embarassing hack....tired;)
int numMoves[NUM_PIECES] = {3,3,6,4,16,16,2};

struct piece {
    char name[NAME_SIZE];
    int idx;
    int side;
    int numPieces;
    struct pos startPos;
    struct pos currPos;
};


struct piece pieces[NUM_PIECES] = {
    {{'p','\0'},0,0,8,8,47,8,47},
    {{'r','\0'},1,0,2,0,55,0,55},
    {{'k','\0'},2,0,2,1,57,1,57},
    {{'b','\0'},3,0,2,2,58,2,58},
    {{'Q','\0'},4,0,1,5,59,5,59},
    {{'K','\0'},5,0,1,6,50,6,50},
    {{' ','\0'},6,-1,32,2,0,2,0}
};

struct feasibleMoves {
    struct move feasibleMoves[MAX_MOVES];
};

struct side {
    int side;
    char color[10];
    char compStrategy[20];
    char selfStrategy[20];
};

int getBoardIdx(struct pos p)
{
    return (int) (p.row*8)+(p.col);
}

int idx;

struct pos getPos(struct pos result, int side, int id, int idx, int numPieces)
{
    int row,col;

    switch (numPieces){
	case 32:
	    row = (int) (16 + idx)/8;
	    col = (int) idx%8;
	    result = (struct pos) {row,col};
	    break;
        case 8:
	    row = (side == 0) ? 1 : 6;
            result = (struct pos) {row,idx};	    
            break;
	case 2:
            row = (side == 0) ? 0 : 7;
	    result = (idx == 0) ? (struct pos) {row,id-1} : (struct pos) {row,8-id};
	    break;
	default:
	    row = (side == 0) ? 0 : 7;
	    result = (struct pos) {row,id-1};
	    break;
    }
    return result;
}

char *prefix[2] = {"w_","b_"};
char *sides[2]  = {"0","1"};
char name[NAME_SIZE];

struct piece* initBoard()
{
    struct piece *board = (struct piece *) malloc(BOARD_SIZE*sizeof(struct piece));

    for (int side=0; side<NUM_SIDES; side++)
    {
        for (int i=0; i<NUM_PIECES;i++)
        {
	   int numPieces = pieces[i].numPieces;

           for (int j=0; j<numPieces; j++)
	   {
	      struct pos pos;
	      pos = getPos(pos, side, i, j, numPieces);
	      int idx = getBoardIdx(pos); 
	      char buffer[2] = "";
	      if (pieces[i].side!=-1){
		  sprintf(buffer,"%d",j);
	          strcat(board[idx].name,prefix[side]);
	      }
	      strcat(board[idx].name,pieces[i].name);
	      strcat(board[idx].name,buffer);
	      board[idx].idx = i;
	      if (idx < 16)
	          board[idx].side = 0;
	      else if (idx > 47)
		  board[idx].side = 1;
	      else
		  board[idx].side = -1;
	      board[idx].numPieces = numPieces;
	      board[idx].startPos = pos;
	      board[idx].currPos = pos;
	      //printf("{%s,%d,%d,{%d,%d},{%d,%d}\n",board[idx].name,board[idx].side,board[idx].numPieces,board[idx].startPos.row,board[idx].startPos.col,board[idx].currPos.row,board[idx].currPos.col);
	      }
	   }
	}
    return board;
}

struct move *getFeasibleMoves(struct piece *piece)
{
    struct move *candidateMoves = moves[piece->idx];

    int num_moves = (int) numMoves[piece->idx];

    int flag = (piece->side == 1) ? -1 : 1;

    struct move *feasibleMoves = (struct move *) calloc(num_moves,sizeof(struct move));
    
    for (int i=0; i<num_moves; i++)
    {
	feasibleMoves[i] = (struct move) {flag*candidateMoves[i].rowChange,flag*candidateMoves[i].colChange};
    }

    return feasibleMoves;
}


int main()
{
    struct piece *board = initBoard();

    struct feasibleMoves *feasibleMoves = (struct feasibleMoves *) calloc(MAX_MOVES,sizeof(struct move));

    for (int i=0; i<BOARD_SIZE; i++)
    {
        if (board[i].side!=EMPTY_FLAG){
            //printf("{%s,idx: %d,%d,%d,{%d,%d},{%d,%d})\n",board[i].name,board[i].idx,board[i].side,board[i].numPieces,board[i].startPos.row,board[i].startPos.col,board[i].currPos.row,board[i].currPos.col);
	    int idx = board[i].idx;
	    struct move *feasMoves = getFeasibleMoves(&board[i]);
	    printf("---------------%s-------------------\n",board[i].name);
            for (int k=0; k<numMoves[idx]/2; k++){
                printf("move[%d]:\t{%d,%d}\t",k,feasMoves[k].rowChange,feasMoves[k].colChange);
	    }
	    printf("\n");
        }
    }
}

