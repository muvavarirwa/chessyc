#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BOARD_SIZE 64
#define NAME_SIZE 10
#define MAX_MOVES 10
#define BYTE_SIZE 8
#define NUM_PIECES 7
#define TUPLE 2
#define NUM_PAWNS

struct pos {
    int row;
    int col;
};
    
struct move {
    int rowChange;
    int colChange;
};

struct move pawnMoves[]   = {1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1};
struct move rookMoves[]   = {0,1,1,0,0,-1,-1,0,0,0,0,0,0,0,0,0};
struct move knightMoves[] = {1,2,2,1,1,-2,-2,1,0,0,0,0,0,0,0,0};
struct move bishopMoves[] = {1,1,1,-1,-1,1,-1,-1,0,0,0,0,0,0,0,0};
struct move queenMoves[]  = {1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1};
struct move kingMoves[]   = {1,1,1,-1,-1,1,-1,-1,1,0,0,1,-1,0,0,-1};
struct move emptyMoves[]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

struct piece {
    char name[NAME_SIZE];
    int side;
    int numPieces;
    struct pos startPos;
    struct pos currPos;
    struct move *moves[8];
} piecesA[NUM_PIECES] = {
    {{'p','\0'},0,8,8,47,8,47, pawnMoves},
    {{'r','\0'},0,2,0,55,0,55, rookMoves},
    {{'k','\0'},0,2,1,57,1,57, knightMoves},
    {{'b','\0'},0,2,2,58,2,58, bishopMoves},
    {{'Q','\0'},0,1,5,59,5,59, queenMoves},
    {{'K','\0'},0,1,6,50,6,50, kingMoves},
    {{' ','\0'},-1,32,2,0,2,0, emptyMoves}
};

struct feasibleMoves {
    int side0[MAX_MOVES];
    int side1[MAX_MOVES];
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

char *pieces[] = {"p","r","k","b","Q","K",""};

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
    struct piece *board = (struct piece *) calloc(BOARD_SIZE,sizeof(struct piece));

    for (int side=0; side<2; side++)
    {
        for (int i=0; i<NUM_PIECES;i++)
        {
	   int numPieces = piecesA[i].numPieces;

           for (int j=0; j<numPieces; j++)
	   {
	      struct pos pos;
	      struct piece *piece = (struct piece *) malloc(sizeof(struct piece));
	      pos = getPos(pos, side, i, j, numPieces);
	      int idx = getBoardIdx(pos); 
	      printf("piece:\t\t%s\t{%d,%d}\n",pieces[i],pos.row,pos.col);
	      char buffer[2] = "";
	      if (piecesA[i].side!=-1){
		  sprintf(buffer,"%d",j);
	          strcat(piece->name,prefix[side]);
	      }
	      strcat(piece->name,pieces[i]);
	      strcat(piece->name,buffer);
	      printf("3. piece->name:\t%s\n",piece->name);
	      piece->side = piecesA[i].side;
	      piece->numPieces = numPieces;
	      piece->startPos = pos;
	      piece->currPos = pos;
	      //*piece->moves = *piecesA[idx].moves;
              memcpy(piece->moves,&piecesA[idx].moves,sizeof(piecesA[idx].moves));
	      //printf("piece->moves[0].rowChange:\t%d\n",piece->moves[0]->rowChange);
	      board[idx] = *piece;
	      //printf("board[%d].name\t%s\n", idx,board[idx].name);
	      }
	   }
	}
    return board;
}

int main()
{
    printf("-------------------------------------------------------------------\n");
    struct piece piece;
    printf("sizeof(piece):\t%ld bytes\n",sizeof(piece));
    
    struct piece *ptr;
    printf("sizeof(ptr):\t%ld bytes\n",sizeof(ptr));

    struct piece *board = initBoard();

    printf("sizeof(board):\t%ld bytes\n",sizeof(board));

   printf("-------------------------------------------------------------------\n");

    //for (int i=0; i<64; i++)
    //{
    //    if (board[i].name){
	//printf("{%s,%d,%d,{%d,%d},{%d,%d})\n",board[i].name,board[i].side,board[i].numPieces,board[i].startPos.row,board[i].startPos.col,board[i].currPos.row,board[i].currPos.col);
        //printf("board[%d].moves[0]\t,board[%d].name,%s\t,{%d,%d}\n",i,i,board[i].name, board[i].moves[0]->rowChange,board[i].moves[0]->colChange);
	//printf("board[%d].name:\t%s\n",i,board[i].name);
    //}
    //}
}

