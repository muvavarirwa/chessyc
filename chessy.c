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
struct move knightMoves[8] = {1,2,1,-2,2,1,2,-1,-1,-2,-1,2,-2,-1,-2,1};
struct move bishopMoves[4] = {1,1,1,-1,-1,1,-1,-1};
struct move queenMoves[16] = {1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1,1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1};
struct move kingMoves[16]  = {1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1,1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1};
struct move emptyMoves[2]  = {0,0,0,0};

struct move *moves[NUM_PIECES] = {pawnMoves, rookMoves, knightMoves, bishopMoves, queenMoves, kingMoves, emptyMoves}; 

//embarassing hack....tired;)
int numMoves[NUM_PIECES] = {3,3,8,4,16,16,2};

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

struct feasibleMove {
    struct pos currPos;
    struct pos nextPos;
};

struct feasibleMoves {
    struct feasibleMove side0[MAX_MOVES];
    struct feasibleMove side1[MAX_MOVES];
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
	      }
	   }
	}
    return board;
}

void getFeasibleMoves(struct piece *board,int i,int idx, struct feasibleMoves *feasibleMoves)
{
    struct piece piece = board[i];
    struct move *candidateMoves = moves[piece.idx];
    int num_moves = (int) numMoves[piece.idx];
    int flag;
   
    switch(piece.side){
        case 0:
	   flag = 1;
           break;
        case 1:
	   flag = -1;
	   break;
	default:
	   flag = 0;
	   break;
        }
    
    for (int i=0; i<num_moves; i++)
    {
	struct pos currPos = piece.currPos;
	struct move move = (struct move) {flag*candidateMoves[i].rowChange,flag*candidateMoves[i].colChange};
	struct pos nextPos = (struct pos) {currPos.row+move.rowChange,currPos.col+move.colChange};
	struct feasibleMove fMove = (struct feasibleMove) {currPos,nextPos};
	int nextIdx = (int) nextPos.row*8 + nextPos.col;
	if (nextPos.row >= 0 && nextPos.row < 8 && nextPos.col >= 0 && nextPos.col < 8) 
	    if (board[nextIdx].side != piece.side && nextPos.row  != currPos.row && nextPos.col != currPos.col && move.rowChange != 0 && move.colChange != 0 ){
		
                if (piece.side == 0){
		    memcpy(feasibleMoves->side0,&fMove,sizeof(fMove)/sizeof(struct feasibleMove));
		    //feasibleMoves->side0[i] = fMove;
		    //printf("SIDE0\t{%d,%d}\t=====>\t{%d,%d}\n",feasibleMoves->side0[i].currPos.row,feasibleMoves->side0[i].currPos.col,feasibleMoves->side0[i].nextPos.row,feasibleMoves->side0[i].nextPos.col);
		}
	        else {
		    memcpy(feasibleMoves->side1,&fMove,sizeof(fMove)/sizeof(struct feasibleMove));
		    //feasibleMoves->side1[i] = fMove;
                    //printf("SIDE1\t{%d,%d}\t=====>\t{%d,%d}\n",feasibleMoves->side1[i].currPos.row,feasibleMoves->side1[i].currPos.col,feasibleMoves->side1[i].nextPos.row,feasibleMoves->side1[i].nextPos.col);
		}
            }
    }

}

void *getAllFeasibleMoves(struct piece *board, struct feasibleMoves *feasibleMoves)
{
    struct feasibleMoves *feasMoves;
 
    for (int i=0; i<BOARD_SIZE; i++)
    {
	int idx = board[i].idx;
	if (board[i].side != -1){
	    struct feasibleMoves *feasMoves = (struct feasibleMoves *) calloc(numMoves[board[i].idx],sizeof(struct feasibleMove));
	    getFeasibleMoves(board,i,idx,feasMoves);
	    memcpy(feasibleMoves,feasibleMoves,sizeof(*feasibleMoves)/sizeof(struct feasibleMove)); 
	    free(feasMoves);
           }
    }

    return feasibleMoves;
}

int main()
{
    struct piece *board = initBoard();
    struct feasibleMove *ptr;
    struct feasibleMoves *feasibleMoves = (struct feasibleMoves *) calloc(MAX_MOVES,sizeof(struct feasibleMove));
    struct feasibleMoves *feasibleMoves_ = getAllFeasibleMoves(board,feasibleMoves);

    int numFeasibleMoves = sizeof(*feasibleMoves_)/sizeof(feasibleMoves_->side0[0]);
    
    printf("sizeof(*feasibleMoves):\t%d\n",numFeasibleMoves);
    
    for (int j=0; j<NUM_SIDES; j++){
	printf("-------------- SIDE: %d ----------------\n",j);
	ptr = (j==0) ? &feasibleMoves_->side0[0] : &feasibleMoves_->side1[0];
        for (int i=0;i<numFeasibleMoves;i++){
            printf("{%d,%d}\t===>\t{%d,%d}\n",ptr[i].currPos.row,ptr[i].currPos.col,ptr[i].nextPos.row,ptr[i].nextPos.col);
	}
    }

    return 0;
    
}

