#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int player_turn=0; // 1 for white; 2 for black; 3 is used to determine the save format (3 is for pvc)
int saved_game_state=0; // 0 if no saved game, 12 if there is a pvp saved game 3 if there is a pvc saved game


// P-pawns R-rook B-bishop N-knight Q-queen K-king for white
// p-pawns r-rook b-bishop n-knight q-queen k-king for black

//board dimension
const unsigned DIM=9;

//general functions
void make_board(char* board[][DIM]);
void print_board(char* board[][DIM]);
void move(char* board[][DIM]);
void move_pvp(char* board[][DIM], int player_turn);
int gen_move(char* board[][DIM], char piece_name, int init_i, int init_j);
void decode_i_j_to_coord(int fin_i, int fin_j, char* result);

//check functions
int pawn_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM]);
int king_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM]);
int rook_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM]);
int knight_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM]);
int bishop_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM]);
int queen_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM]);

//seach functions
int search_boardW(char* board[][DIM], int *coordarrayW);
int search_boardB(char* board[][DIM], int *coordarrayB);

//coordinates array
int *coordarrayW;
int *coordarrayB;


//nr of pieces array (shanon)
int nrpieces_arrayW[6];
int nrpieces_arrayB[6];

// Shanon functions
int mobilityW(char* board[][DIM], int *coordarrayW);
int mobilityB(char* board[][DIM], int *coordarrayB);
void nr_pieceW(char* board[][DIM], int *coordarrayW, int* nrpieces_arrayW);
void nr_pieceB(char* board[][DIM], int *coordarrayB, int* nrpieces_arrayB);
int doubled_pawnW(char* board[][DIM], int *coordarrayW);
int doubled_pawnB(char* board[][DIM], int *coordarrayB);
int isolated_pawnW(char* board[][DIM], int *coordarrayW);
int isolated_pawnB(char* board[][DIM], int *coordarrayB);
int blocked_pawnW(char* board[][DIM], int *coordarrawW);
int blocked_pawnB(char* board[][DIM], int *coordarrayB);

int moves_W[16];
int moves_B[16];

int *best_moves_arrayB;

int best_movesB(char* board[][DIM], int *coordarrayB, int *coordarrayW, int *nrpieces_arrayB, int *nrpieces_arrayW,
                int *best_moves_arrayB);
//int best_movesW(char* board[][DIM], int* coordarrayW[][16]);
void gen_move_S(char* board[][DIM], int init_i, int init_j, char piece_name, int *coordarrayW, int *coordarrayB, 
                int* nrpieces_arrayW, int* nrpieces_arrayB, int *best_moves_arrayB, int k);

int check_check(char* board[][DIM], int *coordarrayW, int* coordarrayB);
int check_mate(char* board[][DIM], int* coordarrayW, int* coordarrayB);

int save_game(char* board[][DIM], int player_turn, int saved_game_state, FILE* fin);
int load_game(char* board[][DIM], int player_turn, int saved_game_state, FILE* fin);

FILE* fin;

int main()
{
      fin=fopen("chess_save.txt","r+");

      if(fin==NULL)
      {
        printf("could not open the save file\n");
        return 0;
      }
      
      coordarrayB=(int*)malloc(16*3*sizeof(int));
      coordarrayW=(int*)malloc(16*3*sizeof(int));
      if((coordarrayB==NULL) || (coordarrayW==NULL))
      {
        printf("could not allocate memory for coordinate array\n");
        return 0;
      }

      best_moves_arrayB=(int*)malloc(16*6*sizeof(int));
      
      char* board[DIM][DIM];

      printf("Choose the game mode:\n");
      printf("press 1 for PVP\n");
      printf("press 2 for PVC\n");
      printf("press 3 to load the last game\n");
      printf("while in game type sav to save the game\n !!! only one save will be stored at a time\n !!! old saves will be overwritten !!!\n");
      printf("press 0 to exit\n");

      int gmod;
      scanf("%i", &gmod);

      switch(gmod)
      {
        case 1://pvp
        {
                player_turn=1;
                make_board(board);
                print_board(board);

                move_pvp(board, player_turn);

                break;
        }
        case 2://pvc
        {
                player_turn=3;
                make_board(board);
                print_board(board);

                search_boardW(board, coordarrayW);
                search_boardB(board, coordarrayB);

                move(board);
                free(coordarrayB);
                free(coordarrayW);
                free(best_moves_arrayB);

                break;

        }
        case 0://exit
        {
                printf("exiting.\nexiting..\nexiting...\n");
                printf("task ended successfully");
                exit(0);
                break;
        }
        case 3://load
        {
                //int player_turn=0; // 1 for white; 2 for black; 3 is used to determine the save format (3 is for pvc)
                //int saved_game_state=0; // 0 if no saved game, 12 if there is a pvp saved game 3 if there is a pvc saved game
                // pturn
                //g state
                //board

                fscanf(fin, "%i\n", &player_turn);
                printf("the player turn is %i\n", player_turn);
                fscanf(fin, "%i\n", &saved_game_state);
                printf("the g state is %i\n", saved_game_state);
                char aux;
                for(int i=0; i<9; i++)
                {
                        for(int j=0; j<9; j++)
                        {
                                fscanf(fin, "%c ", &aux);
                                if(aux=='s') board[i][j]=' ';
                                else
                                             board[i][j]=aux;
                                //printf("%c ", aux);
                        }
                        printf("\n");
                }

                if(saved_game_state==0)
                {
                        printf("no saved game");
                }
                else if(saved_game_state==12)
                {
                       
                        print_board(board);

                        move_pvp(board, player_turn);
                }
                else if(saved_game_state==3)
                {
                        
                        print_board(board);

                        search_boardW(board, coordarrayW);
                        search_boardB(board, coordarrayB);

                        move(board);
                        free(coordarrayB);
                        free(coordarrayW);
                        free(best_moves_arrayB);    
                }

                break;
        }

      }

      make_board(board);
      print_board(board);

      search_boardW(board, coordarrayW);
      search_boardB(board, coordarrayB);
     
        /*for(int j=0; j<=15; j++)
        {
                printf("the piece is %c and is found at i=%i j=%i \n", coordarrayW[0][j], coordarrayW[1][j], coordarrayW[2][j]);
        }*/

      // printf("the mob of white is %i and the mob of black is %i \n", mobilityW(board, coordarrayW), mobilityB(board, coordarrayB));
      // printf("isolated white %i, isolated black %i \n", isolated_pawnW(board, coordarrayW), isolated_pawnB(board, coordarrayB));
      // printf("blocked white %i, blocked black %i \n", blocked_pawnW(board, coordarrayW), blocked_pawnB(board, coordarrayB));
      // printf("doubled white %i, doubled black %i \n", doubled_pawnW(board, coordarrayW), doubled_pawnB(board, coordarrayB));
        /*
         nr_pieceW(board, coordarrayW, nrpieces_arrayW);
         nr_pieceB(board, coordarrayB, nrpieces_arrayB);

        int found=search_boardB(board, coordarrayB);
                for(int i=0; i<found; i++)
                {
                        printf("the black board is %c, %i, %i \n", coordarrayB[0][i], coordarrayB[1][i], coordarrayB[2][i]);
                }
                //search_boardW(board, coordarrayW);
        */
       // int name=0;
       // int i_coord=1;
       // int j_coord=2;
        //int found=search_boardB(board, coordarrayB);
        
       // move(board);
       // free(coordarrayB);
       // free(coordarrayW);
       // free(best_moves_arrayB);
    
        return 0;
}

void make_board(char* board[][DIM])
{
        for(int i=0; i<DIM; i++)
                for(int j=0; j<DIM; j++)
                {
                        board[i][j]=' ';
                }
        
        char nr='8';
        char lett='a';
        for(int i=0; i<DIM-1; i++)
        {
                board[i][0]=nr-i;
        }
        for(int j=1; j<DIM; j++)
        {
                board[DIM-1][j]=lett+(j-1);
        }
        
        //P-pawns i== 1 OR i == 6    and j form 1 to dim-1
        for(int j=1; j<DIM; j++)
        {
                board[1][j]='p';
                board[6][j]='P';
        }
        
        //R-rook
        board[0][1]='r';
        board[0][8]='r';
        board[7][1]='R';
        board[7][8]='R';
        
        //N-knight
        board[0][2]='n';
        board[0][7]='n';
        board[7][2]='N';
        board[7][7]='N';
        
        //B-bishop
        board[0][3]='b';
        board[0][6]='b';
        board[7][3]='B';
        board[7][6]='B';
        
        //Q-queen
        board[0][4]='q';
        board[7][4]='Q';
        
        //K-king
        board[0][5]='k';
        board[7][5]='K';
}
void print_board(char* board[][DIM])
{
      for(int i=0; i<DIM; i++)
      {
                for(int j=0; j<DIM; j++)
                {
                        
                        //if(((i+j)%2)==1) setbkcolor(WHITE 47)(magenta 45)(green 42);
                      // else
                        //        setbkcolor(BLACK 40);
                        // 34 blue
                        
                        if(((i+j)%2)==1 && j>0 && i<8) 
                                printf("\033[37m\033[42m"); // white tiles are green
                        else
                                printf("\033[37m\033[40m"); // black tiles are black
                                
                        if(j==0 || i==8)
                                printf("\033[37m\033[34m"); //coordinates are blue
                                
                        printf("%c ", board[i][j]);
                }
               printf("\033[0m"); //reset colour
                printf("\n");      //endl at endof board else a long snek
        } 
}
void move(char* board[][DIM])
{
        char init[4];
        char final[4];
        do
        {
      
        
                //the accepted format is INITIAL_POZ-FINAL_POZ
                // INITIAL_POZ and/or FINAL_POZ being PIECE_NAME_LETTERCOORD_NUMBERCOORD
                //EXAMPLE PA2-PA3

                 if(check_mate(board, coordarrayW, coordarrayB)==1)
                {
                        printf("black wins!");
                        exit(0);
                }
                else if(check_mate(board, coordarrayW, coordarrayB)==2)
                {
                        printf("white wins!");
                        exit(0);
                }
                
                scanf("%3s-%3s", init, final);
                
                if(strcmp(init, "st")==0) exit(0);
                
                //printf("%s goes to %s \n", init, final);
                
                int init_i, init_j, fin_i, fin_j;
                
                int check=0;
                
                char piece_name;
                /*piece_name=init[0];
                
                init_j=init[1]-'A'+1;
                fin_j=final[1]-'A'+1;
                
                init_i=abs(init[2]-'8');
                fin_i=abs(final[2]-'8');
                */
                
                if(strcmp(init, "sav")==0)
                {
                        save_game(board, player_turn, saved_game_state, fin);
                        printf("game saved");
                        exit(0);
                }
                else
                {
                        printf("%s goes to %s \n", init, final);
                        piece_name=init[0];
                        init_j=init[1]-'A'+1;
                        fin_j=final[1]-'A'+1;
                
                        init_i=abs(init[2]-'8');
                        fin_i=abs(final[2]-'8');

                        if(piece_name=='p' || piece_name=='P')
                        {
                                check=pawn_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                        }
                        else
                        {
                                if(piece_name=='r' || piece_name=='R')
                                {
                                        check=rook_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                }
                                else
                                {
                                if(piece_name=='k' || piece_name=='K')
                                        {
                                                check=king_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        }
                                        else
                                        {
                                                if(piece_name=='n' || piece_name=='N')
                                                {
                                                        check=knight_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                                }
                                                else
                                                {
                                                        if(piece_name=='b' || piece_name=='B')
                                                        {
                                                                check=bishop_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                                        }
                                                        else
                                                        {
                                                                if(piece_name=='q' || piece_name=='Q')
                                                                {
                                                                        check=queen_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                        
                        if(check)
                        {
                                board[fin_i][fin_j]=board[init_i][init_j];
                                board[init_i][init_j]=' ';
                                print_board(board);

                        // printf("the mob of white is %i and the mob of black is %i \n", mobilityW(board, coordarrayW), mobilityB(board, coordarrayB));
                                //printf("isolated white %i, isolated black %i \n", isolated_pawnW(board, coordarrayW), isolated_pawnB(board, coordarrayB));
                        // printf("blocked white %i, blocked black %i \n", blocked_pawnW(board, coordarrayW), blocked_pawnB(board, coordarrayB));
                        // printf("doubled white %i, doubled black %i \n", doubled_pawnW(board, coordarrayW), doubled_pawnB(board, coordarrayB));

                        


                                best_movesB(board, coordarrayB, coordarrayW, nrpieces_arrayB, nrpieces_arrayW, best_moves_arrayB);
                        }
                        else
                {
                        printf("impossible move \n");
                }
                }
        }while(strcmp(init, "st")!=0);

       /* while(strcmp(init, "st")!=0)
        {
                move(board);
        }
        */
        
}

int pawn_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM])
{
        int check=0;
        if((fin_i<=7 && fin_i>=0) && (fin_j<=8 && fin_j>=1))
        {
                if(piece_name=='P')
                {
                        //check if the line is good(move up an white)
                        if(fin_i==(init_i-1))
                        {
                                //check if it moves forward
                                if(fin_j==init_j)
                                {
                                        //check if forward is empty
                                        if(board[fin_i][fin_j]==' ') check=1;
                                }
                                else
                                {
                                        //if move diagonaly and diagonal is enemy
                                        if((abs(fin_j-init_j)==1) && islower(board[fin_i][fin_j])) check=1; 
                                }
                        }
                }
                else
                {
                        if(piece_name=='p')
                        {
                                //check if moves down as black
                                if(fin_i==(init_i+1))
                                {
                                        //check if it moves forward
                                        if(fin_j==init_j)
                                        {
                                                //check if forward is empty
                                                if(board[fin_i][fin_j]==' ') check=1;
                                        }
                                        else
                                        {
                                                //if move diagonaly and diagonal is enemy
                                                if((abs(fin_j-init_j)==1) && isupper(board[fin_i][fin_j])) check=1;
                                        }
                                }
                        }
                }
        }
        
        return check;
}
int king_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM])
{
        int check=0;
        if((fin_i<=7 && fin_i>=0) && (fin_j<=8 && fin_j>=1))
        {
                if(piece_name=='K')
                {
                        if((abs(fin_i-init_i)<=1) && (abs(fin_j-init_j<=1)))
                        {
                                if((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j])))
                                {
                                        check=1;
                                }
                        }
               }
                else
                {
                        if(piece_name=='k')
                        {
                                if((abs(fin_i-init_i)<=1) && (abs(fin_j-init_j<=1)))
                                {
                                        if((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j])))
                                        {
                                                check=1;
                                        }
                                }
                        }
                }
        }
        return check;
}
int rook_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM])
{
        int check=0;
        if((fin_i<=7 && fin_i>=0) && (fin_j<=8 && fin_j>=1))
        {
                if(piece_name=='R')//WHITE
                {
                        //general check
                        if((fin_i==init_i) || (fin_j==init_j))
                        {
                                //if it moves left or right
                                if(fin_i==init_i)
                                {
                                        if(fin_j>init_j) //if right
                                        {
                                                int obstacle=0;
                                                for(int k=init_j+1; k<fin_j && (obstacle==0); k++)
                                                {
                                                        if(board[fin_i][k]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                                
                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                {
                                                        check=1;
                                                }
                                        }
                                        else //if left fin_j<init_j
                                        {
                                                int obstacle=0;
                                                for(int k=init_j-1; k>fin_j && (obstacle==0); k--)
                                                {
                                                        if(board[fin_i][k]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                                
                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                {
                                                        check=1;
                                                }
                                        }
                                }
                                else
                                {
                                        if(fin_j==init_j)//if it moves up or down
                                        {
                                                if(fin_i<init_i)//if up
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_i-1; k>fin_i && (obstacle==0); k--)
                                                        {
                                                                if(board[k][fin_j]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                        //printf("obstacle");
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                                else //if down fin_i>init_i
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_i+1; k<fin_i && (obstacle==0); k++)
                                                        {
                                                                if(board[k][fin_j]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                        }
                                }
                        }
                }
                if(piece_name=='r')//BLACK
                {
                  //general check
                        if((fin_i==init_i) || (fin_j==init_j))
                        {
                                //if it moves left or right
                                if(fin_i==init_i)
                                {
                                        if(fin_j>init_j) //if right
                                        {
                                                int obstacle=0;
                                                for(int k=init_j+1; k<fin_j && (obstacle==0); k++)
                                                {
                                                        if(board[fin_i][k]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                                
                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                {
                                                        check=1;
                                                }
                                        }
                                        else //if left fin_j<init_j
                                        {
                                                int obstacle=0;
                                                for(int k=init_j-1; k>fin_j && (obstacle==0); k--)
                                                {
                                                        if(board[fin_i][k]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                                
                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                {
                                                        check=1;
                                                }
                                        }
                                }
                                else
                                {
                                        if(fin_j==init_j)//if it moves up or down
                                        {
                                                if(fin_i<init_i)//if up
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_i-1; k>fin_i && (obstacle==0); k--)
                                                        {
                                                                if(board[k][fin_j]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                        //printf("obstacle");
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                                else //if down fin_i>init_i
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_i+1; k<fin_i && (obstacle==0); k++)
                                                        {
                                                                if(board[k][fin_j]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                        }
                                }
                        } 
                }    
        }
        return check;
}
int knight_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM])
{
        int check=0;
        if((fin_i<=7 && fin_i>=0) && (fin_j<=8 && fin_j>=1))
        {
                if(piece_name=='N')
                {
                        if( ((abs(fin_i-init_i)==1) && (abs(fin_j-init_j)==2)) || ((abs(fin_i-init_i)==2) && (abs(fin_j-init_j)==1)) )
                        {
                                if(islower(board[fin_i][fin_j]) || (board[fin_i][fin_j]==' '))
                                {
                                        check=1;
                                }
                        }
                }
                else
        {
                if(piece_name=='n')
                {
                        if( ((abs(fin_i-init_i)==1) && (abs(fin_j-init_j)==2)) || ((abs(fin_i-init_i)==2) && (abs(fin_j-init_j)==1)) )
                        {
                                if(isupper(board[fin_i][fin_j]) || (board[fin_i][fin_j]==' '))
                                {
                                        check=1;
                                }
                        }
                }
        }
        }
        return check;
}
int bishop_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM])
{
        int check=0;
        if((fin_i<=7 && fin_i>=0) && (fin_j<=8 && fin_j>=1))
        {
                if(piece_name=='B')//white
                {
                        if((abs(fin_i-init_i))==(abs(fin_j-init_j)))
                        {
                                int obstacle=0;
                                //diag right to left down to up like backslash
                                if(fin_i<init_i && fin_j<init_j)
                                {
                                        for(int k=init_i-1, l=init_j-1; k>fin_i && l>fin_j && obstacle==0; k--, l--)
                                        {
                                                if(board[k][l]!=' ')
                                                {
                                                obstacle=1;
                                                //printf("//obs");
                                                }
                                        }
                                }
                                else
                                {
                                        //diag left to right up to down
                                        if(fin_i>init_i && fin_j>init_j)
                                        {
                                                for(int k=init_i+1, l=init_j+1; k<fin_i && l<fin_j && obstacle==0; k++, l++)
                                                {
                                                        if(board[k][l]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                //diag left to right    down to up
                                                if(fin_i<init_i && fin_j>init_j)
                                                {
                                                        for(int k=init_i-1, l=init_j+1; k>fin_i && l<fin_j && obstacle==0; k--, l++)
                                                        {
                                                                if(board[k][l]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        //diag right to left up to down
                                                        if(fin_i>init_i && fin_j<init_j)
                                                        {
                                                                for(int k=init_i+1, l=init_j-1; k<fin_i && l>fin_j && obstacle==0; k++, l--)
                                                                {
                                                                        if(board[k][l]!=' ')
                                                                        {
                                                                                obstacle=1;
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                                //printf("pre if value of obstacle is %i \n", obstacle);
                                if(obstacle==0)
                                {
                                        if(islower(board[fin_i][fin_j]) || board[fin_i][fin_j]==' ')
                                        {
                                                //printf("the value of obstacle is %i \n", obstacle);
                                                check=1;
                                        }
                                       
                                }
                        }
                }
                else //black
                {
                        if((abs(fin_i-init_i))==(abs(fin_j-init_j)))
                        {
                                int obstacle=0;
                                //diag right to left down to up
                                if(fin_i<init_i && fin_j<init_j)
                                {
                                        for(int k=init_i-1, l=init_j-1; k>fin_i && l>fin_j && obstacle==0; k--, l--)
                                        {
                                                if(board[k][l]!=' ')
                                        {
                                                        obstacle=1;
                                                }
                                        }
                                }
                                else
                                {
                                        //diag left to right up to down
                                        if(fin_i>init_i && fin_j>init_j)
                                        {
                                                for(int k=init_i+1, l=init_j+1; k<fin_i && l<fin_j && obstacle==0; k++, l++)
                                                {
                                                        if(board[k][l]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                //diag left to right    down to up
                                                if(fin_i<init_i && fin_j>init_j)
                                                {
                                                        for(int k=init_i-1, l=init_j+1; k>fin_i && l<fin_j && obstacle==0; k--, l++)
                                                        {
                                                                if(board[k][l]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        //diag right to left up to down
                                                        if(fin_i>init_i && fin_j<init_j)
                                                        {
                                                                for(int k=init_i+1, l=init_j-1; k<fin_i && l>fin_j && obstacle==0; k++, l--)
                                                                {
                                                                        if(board[k][l]!=' ')
                                                                        {
                                                                                obstacle=1;
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                                
                                if((obstacle==0) && ((isupper(board[fin_i][fin_j])) || board[fin_i][fin_j]==' '))
                                {
                                        check=1;
                                }
                        }
                }
        }
        //printf("the value of check is %i \n",check);
        return check;
}
int queen_check(int init_i, int init_j, int fin_i, int fin_j, char piece_name, char* board[][DIM])
{
        int check=0;
        if((fin_i<=7 && fin_i>=0) && (fin_j<=8 && fin_j>=1))
        {
                if(piece_name=='Q')//white
                {
                        //general check rook movement
                        if((fin_i==init_i) || (fin_j==init_j))
                        {
                                //if it moves left or right
                                if(fin_i==init_i)
                                {
                                        if(fin_j>init_j) //if right
                                        {
                                                int obstacle=0;
                                                for(int k=init_j+1; k<fin_j && (obstacle==0); k++)
                                                {
                                                        if(board[fin_i][k]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                                
                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                {
                                                        check=1;
                                                }
                                        }
                                        else //if left fin_j<init_j
                                        {
                                                int obstacle=0;
                                                for(int k=init_j-1; k>fin_j && (obstacle==0); k--)
                                                {
                                                        if(board[fin_i][k]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                                
                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                {
                                                        check=1;
                                                }
                                        }
                                }
                                else
                                {
                                        if(fin_j==init_j)//if it moves up or down
                                        {
                                                if(fin_i<init_i)//if up
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_i-1; k>fin_i && (obstacle==0); k--)
                                                        {
                                                                if(board[k][fin_j]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                        //printf("obstacle");
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                                else //if down fin_i>init_i
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_i+1; k<fin_i && (obstacle==0); k++)
                                                        {
                                                                if(board[k][fin_j]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (islower(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                        }
                                }
                        }
                        else //check for bishop movement
                        {
                              if((abs(fin_i-init_i))==(abs(fin_j-init_j)))
                                {
                                        int obstacle=0;
                                        //diag right to left down to up
                                        if(fin_i<init_i && fin_j<init_j)
                                        {
                                                for(int k=init_i-1, l=init_j-1; k>fin_i && l>fin_j && obstacle==0; k--, l--)
                                                {
                                                        if(board[k][l]!=' ')
                                                        {
                                                                obstacle=1;
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                //diag left to right up to down
                                                if(fin_i>init_i && fin_j>init_j)
                                                {
                                                        for(int k=init_i+1, l=init_j+1; k<fin_i && l<fin_j && obstacle==0; k++, l++)
                                                        {
                                                                if(board[k][l]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        //diag left to right    down to up
                                                        if(fin_i<init_i && fin_j>init_j)
                                                        {
                                                                for(int k=init_i-1, l=init_j+1; k>fin_i && l<fin_j && obstacle==0; k--, l++)
                                                                {
                                                                        if(board[k][l]!=' ')
                                                                        {
                                                                                obstacle=1;
                                                                        }
                                                                }
                                                        }
                                                        else
                                                        {
                                                                //diag right to left up to down
                                                                if(fin_i>init_i && fin_j<init_j)
                                                                {
                                                                        for(int k=init_i+1, l=init_j-1; k<fin_i && l>fin_j && obstacle==0; k++, l--)
                                                                        {
                                                                                if(board[k][l]!=' ')
                                                                                {
                                                                                        obstacle=1;
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                        if(obstacle==0 && ((islower(board[fin_i][fin_j])) || board[fin_i][fin_j]==' '))
                                        {
                                                check=1;
                                        }
                                }          
                        }    
                }
                else
                {
                        if(piece_name=='q')//black
                        {
                                //general check rook movement
                                if((fin_i==init_i) || (fin_j==init_j))
                                {
                                        //if it moves left or right
                                        if(fin_i==init_i)
                                        {
                                                if(fin_j>init_j) //if right
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_j+1; k<fin_j && (obstacle==0); k++)
                                                        {
                                                                if(board[fin_i][k]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                                else //if left fin_j<init_j
                                                {
                                                        int obstacle=0;
                                                        for(int k=init_j-1; k>fin_j && (obstacle==0); k--)
                                                        {
                                                                if(board[fin_i][k]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                        
                                                        if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                        {
                                                                check=1;
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                if(fin_j==init_j)//if it moves up or down
                                                {
                                                        if(fin_i<init_i)//if up
                                                        {
                                                                int obstacle=0;
                                                                for(int k=init_i-1; k>fin_i && (obstacle==0); k--)
                                                                {
                                                                        if(board[k][fin_j]!=' ')
                                                                        {
                                                                                obstacle=1;
                                                                                //printf("obstacle");
                                                                        }
                                                                }
                                                                
                                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                                {
                                                                        check=1;
                                                                }
                                                        }
                                                        else //if down fin_i>init_i
                                                        {
                                                                int obstacle=0;
                                                                for(int k=init_i+1; k<fin_i && (obstacle==0); k++)
                                                                {
                                                                        if(board[k][fin_j]!=' ')
                                                                        {
                                                                                obstacle=1;
                                                                        }
                                                                }
                                                                
                                                                if(obstacle==0 && ((board[fin_i][fin_j]==' ') || (isupper(board[fin_i][fin_j]))))
                                                                {
                                                                        check=1;
                                                                }
                                                        }
                                                }
                                        }
                                }
                                else
                                {
                                        //gen check for bishop movement
                                        if((abs(fin_i-init_i))==(abs(fin_j-init_j)))
                                        {
                                                int obstacle=0;
                                                //diag right to left down to up
                                                if(fin_i<init_i && fin_j<init_j)
                                                {
                                                        for(int k=init_i-1, l=init_j-1; k>fin_i && l>fin_j && obstacle==0; k--, l--)
                                                        {
                                                                if(board[k][l]!=' ')
                                                                {
                                                                        obstacle=1;
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        //diag left to right up to down
                                                        if(fin_i>init_i && fin_j>init_j)
                                                        {
                                                                for(int k=init_i+1, l=init_j+1; k<fin_i && l<fin_j && obstacle==0; k++, l++)
                                                                {
                                                                        if(board[k][l]!=' ')
                                                                        {
                                                                                obstacle=1;
                                                                        }
                                                                }
                                                        }
                                                        else
                                                        {
                                                                //diag left to right    down to up
                                                                if(fin_i<init_i && fin_j>init_j)
                                                                {
                                                                        for(int k=init_i-1, l=init_j+1; k>fin_i && l<fin_j && obstacle==0; k--, l++)
                                                                        {
                                                                                if(board[k][l]!=' ')
                                                                                {
                                                                                        obstacle=1;
                                                                                }
                                                                        }
                                                                }
                                                                else
                                                                {
                                                                        //diag right to left up to down
                                                                        if(fin_i>init_i && fin_j<init_j)
                                                                        {
                                                                                for(int k=init_i+1, l=init_j-1; k<fin_i && l>fin_j && obstacle==0; k++, l--)
                                                                                {
                                                                                        if(board[k][l]!=' ')
                                                                                        {
                                                                                                obstacle=1;
                                                                                        }
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }
                                                if(obstacle==0 && ((isupper(board[fin_i][fin_j])) || board[fin_i][fin_j]==' '))
                                                {
                                                        check=1;
                                                }
                                        }
                                } 
                        }
                }
        }
        
        return check;
}

int gen_move(char* board[][DIM], char piece_name, int init_i, int init_j)
{
        int nr_moves=0;
        switch(piece_name)
        {
                case 'p'://for black pawn
                {
                        int check=0; //checking the (3) tiles on the row under, that are in diagonal/under the black pawn
                        for(int fin_i=init_i+1, fin_j=init_j-1; fin_j<=init_j+1; fin_j++)
                        {
                              check=pawn_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                              if(check==1)
                              {
                                /*char result[3];
                                decode_i_j_to_coord(fin_i, fin_j, result);
                                printf("a possible move is %c%s why the under \n", piece_name, result);
                                */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                nr_moves++;
                              }  
                        }
                        return nr_moves;
                        break;
                }
                case 'P'://for white pawn
                {
                        int check=0; //checking the (3) tiles on the row above, that are in diagonal/above the wite pawn
                        for(int fin_i=init_i-1, fin_j=init_j-1; fin_j<=init_j+1; fin_j++)
                        {
                              check=pawn_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                              if(check==1)
                              {
                                /*
                                char result[3];
                                decode_i_j_to_coord(fin_i, fin_j, result);
                                printf("a possible move is %c%s \n", piece_name, result);
                                */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                nr_moves++;
                              }  
                        }
                        return nr_moves;
                        break;
                }
                case 'r'://for black rook
                case 'R'://for white rook (both have the same conditions)
                {
                        int check=0;
                        for(int fin_i=0; fin_i<=7; fin_i++)
                        {
                                for(int fin_j=1; fin_j<=8; fin_j++)
                                {
                                        check=rook_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*
                                          char result[3];
                                          decode_i_j_to_coord(fin_i, fin_j, result);
                                          printf("a possible move is %c%s \n", piece_name, result);
                                          */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                          nr_moves++;
                                        }
                                }
                        }
                        return nr_moves;
                        break;
                }
                case 'b'://for black bishop
                case 'B'://for white bisop (both)
                {
                        int check=0;
                        for(int fin_i=0; fin_i<=7; fin_i++)
                        {
                                for(int fin_j=1; fin_j<=8; fin_j++)
                                {
                                        check=bishop_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*
                                          char result[3];
                                          decode_i_j_to_coord(fin_i, fin_j, result);
                                          printf("a possible move is %c%s \n", piece_name, result);
                                          */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                          nr_moves++;
                                        }
                                }
                        }
                        return nr_moves;
                        break;
                }
                case 'q'://for white queen
                case 'Q'://for black queen
                {
                        int check=0;
                        for(int fin_i=0; fin_i<=7; fin_i++)
                        {
                                for(int fin_j=1; fin_j<=8; fin_j++)
                                {
                                        check=queen_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*
                                          char result[3];
                                          decode_i_j_to_coord(fin_i, fin_j, result);
                                          printf("a possible move is %c%s \n", piece_name, result);
                                          */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                          nr_moves++;
                                        }
                                }
                        }
                        return nr_moves;
                        break;
                }
                case 'n':// for black knight
                case 'N':// for white knight
                {
                        int check=0;
                        for(int fin_i=init_i-2; fin_i<=init_i+2; fin_i++)
                        {
                                for(int fin_j=init_j-2; fin_j<=init_j+2; fin_j++)
                                {
                                        check=knight_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*
                                          char result[3];
                                          decode_i_j_to_coord(fin_i, fin_j, result);
                                          printf("a possible move is %c%s \n", piece_name, result);
                                          */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                          nr_moves++;
                                        }  
                                }
                        }
                        return nr_moves;
                        break;
                }
                case 'k':
                case 'K':
                {
                        int check=0;
                        for(int fin_i=init_i-1; fin_i<=init_i+1; fin_i++)
                        {
                                for(int fin_j=init_j-1; fin_j<=init_j+1; fin_j++)
                                {
                                        check=king_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*
                                          char result[3];
                                          decode_i_j_to_coord(fin_i, fin_j, result);
                                          printf("a possible move is %c%s \n", piece_name, result);
                                          */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                          nr_moves++;
                                        }      
                                }
                        }
                        return nr_moves;
                        break;
                }
                
        }
}

void decode_i_j_to_coord(int fin_i, int fin_j, char* result)
{
        // transforms i and j coord to chess coord 1-8 a-h and puts the coords in an string

        char y_axis_i='8'-fin_i;
        char x_axis_j='A'+fin_j-1;
        
        result[1]=y_axis_i;
        result[0]=x_axis_j;
}

int search_boardW(char* board[][DIM], int *coordarrayW)
{
        /*Searches the board to find all the white pices and puts the coordinates in the coordarrayW matrix*/

        

        int found=0; //nr of found pieces
        int name=0; //the line in the coordarrayW for the piece_name
        int i_coord=1; //the line in the coordarrayW for the i coord
        int j_coord=2; //the line in the coordarrayW for the j coord

        for(int i=0; i<=7; i++)
        {
                for(int j=1; j<=8; j++)
                {
                        if(isupper(board[i][j]))
                        {
                               *(coordarrayW+found)=board[i][j];
                               *(coordarrayW+found+16)=i;
                               *(coordarrayW+found+32)=j;
                               // TEST TO VERIFY
                               //printf("%c %i %i \n", *(coordarrayW+found), *(coordarrayW+found+16), *(coordarrayW+found+32));
                               found++;
                        }
                }
        }
        return found; // returns the nr of found pieces
}
int search_boardB(char* board[][DIM], int *coordarrayB)
{
        

        /*Searches the board to find all the black pices and puts the coordinates in the coordarrayB matrix*/

        int found=0; //nr of found pieces
        int name=0; //the line in the coordarrayB for the piece_name
        int i_coord=1; //the line in the coordarrayB for the i coord
        int j_coord=2; //the line in the coordarrayB for the j coord

        for(int i=0; i<=7 && found!=16; i++)
        {
                for(int j=1; j<=8 && found!=16; j++)
                {
                        if(islower(board[i][j]))
                        {
                               *(coordarrayB+found)=board[i][j];
                               *(coordarrayB+found+16)=i;
                               *(coordarrayB+found+32)=j; 
                               // TEST TO VERIFY
                               //printf("%c %i %i , found is %i\n", *(coordarrayB+found), *(coordarrayB+found+16), *(coordarrayB+found+32), found) ;
                               found++;
                        }
                }
        }
        return found; //returns the nr of found pieces
}

int mobilityW(char* board[][DIM], int *coordarrayW)
{
        //Calculates mobility by generating the moves of the pieces that are in the coordarray

        int mobW=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardW(board, coordarrayW);

        //parsing the coordarray
        for(int i=0; i<found; i++)
        {
                int init_i=*(coordarrayW+16+i);
                int init_j=*(coordarrayW+32+i);
                char piece_name=*(coordarrayW+i);

                mobW=mobW+gen_move(board, piece_name, init_i, init_j);
        }
        return mobW; //returns the mobility W
        
}
int mobilityB(char* board[][DIM], int *coordarrayB)
{
        //Calculates mobility by generating the moves of the pieces that are in the coordarray

        int mobB=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardB(board, coordarrayB);

        //parsing the coordarray
        for(int i=0; i<found; i++)
        {
                int init_i=*(coordarrayB+i+16);
                int init_j=*(coordarrayB+i+32);
                char piece_name=*(coordarrayB+i);

                mobB=mobB+gen_move(board, piece_name, init_i, init_j);
        }

        return  mobB; //returns the mobility B
}

void nr_pieceW(char* board[][DIM], int *coordarrayW, int* nrpieces_arrayW)
{
        //the nrpieces_arrayW vector has the following template
        // 0 1 2 3 4 5
        // P N B R Q K

        int found=search_boardW(board, coordarrayW);
        int name=0;
        int nrpieces_cap=5;

        //initializing the array with 0
        for(int i=0; i<=nrpieces_cap; i++)
        {
                nrpieces_arrayW[i]=0;
        }

        //parsing the coord array to count the nr of pieces on the board
        for(int i=0; i<found; i++)
        {
                int piece_name=*(coordarrayW+i);
                switch(piece_name)
                {
                        case 'P':
                        {
                                nrpieces_arrayW[0]++;
                                break;
                        }

                        case 'N':
                        {
                                nrpieces_arrayW[1]++;
                                break;
                        }           

                        case 'B':
                        {
                                nrpieces_arrayW[2]++;
                                break;
                        }

                        case 'R':
                        {
                                nrpieces_arrayW[3]++;
                                break;
                        }

                        case 'Q':
                        {
                                nrpieces_arrayW[4]++;
                                break;
                        }

                        case 'K':
                        {
                                nrpieces_arrayW[5]++;
                                break;
                        }
                }
        }
}
void nr_pieceB(char* board[][DIM], int *coordarrayB, int* nrpieces_arrayB)
{
        //the nrpieces_arrayW vector has the following template
        // 0 1 2 3 4 5
        // p n b r q k 

        int found=search_boardB(board, coordarrayB);
        int name=0;
        int nrpieces_cap=5;

        //initializing the array with 0
        for(int i=0; i<=nrpieces_cap; i++)
        {
                nrpieces_arrayB[i]=0;
        }

        //parsing the coord array to count the nr of pieces on the board
        for(int i=0; i<found; i++)
        {
                int piece_name=*(coordarrayB+i);
                switch(piece_name)
                {
                        case 'p':
                        {
                                nrpieces_arrayW[0]++;
                                break;
                        }

                        case 'n':
                        {
                                nrpieces_arrayW[1]++;
                                break;
                        }           

                        case 'b':
                        {
                                nrpieces_arrayW[2]++;
                                break;
                        }

                        case 'r':
                        {
                                nrpieces_arrayW[3]++;
                                break;
                        }

                        case 'q':
                        {
                                nrpieces_arrayW[4]++;
                                break;
                        }

                        case 'k':
                        {
                                nrpieces_arrayW[5]++;
                                break;
                        }
                }
        }
}
int doubled_pawnW(char* board[][DIM], int *coordarrayW)
{
        int d_pawnW=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardW(board, coordarrayW);

        for(int i=0; i<found; i++)
        {
                if(*(coordarrayW+i)=='P')
                {
                        int doubled=0;
                        //int pawn_i=coordarrayW[i_coord][i];
                        int pawn_j=*(coordarrayW+32+i);

                        for(int j=0; j<=7 && doubled <2; j++)
                        {
                                if(board[j][pawn_j]=='P')
                                {
                                        doubled++;
                                }
                        }
                        if(doubled>=2)
                        {
                                d_pawnW++;
                        }
                }
        }
        return d_pawnW;
}
int doubled_pawnB(char* board[][DIM], int *coordarrayB)
{
        int d_pawnB=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardB(board, coordarrayB);

        for(int i=0; i<found; i++)
        {
                if(*(coordarrayB+i)=='P')
                {
                        int doubled=0;
                        //int pawn_i=coordarrayB[i_coord][i];
                        int pawn_j=*(coordarrayB+32+i);

                        for(int j=0; j<=7 && doubled <2; j++)
                        {
                                if(board[j][pawn_j]=='P')
                                {
                                        doubled++;
                                }
                        }
                        if(doubled>=2)
                        {
                                d_pawnB++;
                        }
                }
        }
        return d_pawnB;  
}
int isolated_pawnW(char* board[][DIM], int *coordarrayW)
{
        int iso_pawmW=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardW(board, coordarrayW);

        for(int k=0; k<found; k++)
        {
                if(*(coordarrayW+k)=='P')
                {
                        int isolated=0;
                        int pawn_i=*(coordarrayW+16+k);
                        int pawn_j=*(coordarrayW+32+k);

                        for(int i=pawn_i-1; i<=pawn_i+1 && isolated<2; i++)
                        {
                                for(int j=pawn_j-1; j<=pawn_j+1 && isolated<2; j++)
                                {
                                        if(i>=0 && i<=7)
                                        {
                                                if(j>=1 && j<=8)
                                                {
                                                        if(board[i][j]=='P')
                                                        {
                                                                isolated++;
                                                        }
                                                }
                                        }
                                }
                        }
                        
                        if(isolated<2)
                        {
                                iso_pawmW++;
                        }
                }
        }
        return iso_pawmW;        
}
int isolated_pawnB(char* board[][DIM], int *coordarrayB)
{
        int iso_pawmB=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardB(board, coordarrayB);

        for(int k=0; k<found; k++)
        {
                if(*(coordarrayB+k)=='p')
                {
                        int isolated=0;
                        int pawn_i=*(coordarrayB+16+k);
                        int pawn_j=*(coordarrayB+32+k);

                        for(int i=pawn_i-1; i<=pawn_i+1 && isolated<2; i++)
                        {
                                for(int j=pawn_j-1; j<=pawn_j+1 && isolated<2; j++)
                                {
                                        if(i>=0 && i<=7)
                                        {
                                                if(j>=1 && j<=8)
                                                {
                                                        if(board[i][j]=='p')
                                                        {
                                                                isolated++;
                                                        }
                                                }
                                        }
                                }
                        }
                        
                        if(isolated<2)
                        {
                                iso_pawmB++;
                        }
                }
        }
        return iso_pawmB;
}
int blocked_pawnW(char* board[][DIM], int *coordarrayW)
{
        int blk_pawnW=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardW(board, coordarrayW);

        for(int k=0; k<found; k++)
        {
                if(*(coordarrayW+k)=='P')
                {
                        int pawn_i=*(coordarrayW+16+k);
                        int pawn_j=*(coordarrayW+32+k);

                        if(((pawn_i-1)>=0) && (board[pawn_i-1][pawn_j]!=' '))
                        {
                                blk_pawnW++;
                        }

                }
        }
        return blk_pawnW;
}
int blocked_pawnB(char* board[][DIM], int *coordarrayB)
{
        int blk_pawnB=0;
        int name=0;
        int i_coord=1;
        int j_coord=2;
        int found=search_boardB(board, coordarrayB);

        for(int k=0; k<found; k++)
        {
                if(*(coordarrayB+k)=='p')
                {
                        int pawn_i=*(coordarrayB+16+k);
                        int pawn_j=*(coordarrayB+32+k);

                        if(((pawn_i+1)<=7) && (board[pawn_i+1][pawn_j]!=' '))
                        {
                                blk_pawnB++;
                        }
                }
        }
        return blk_pawnB; 
}

void gen_move_S(char* board[][DIM], int init_i, int init_j, char piece_name, int *coordarrayW, int *coordarrayB, 
                int* nrpieces_arrayW, int* nrpieces_arrayB, int *best_moves_arrayB, int k)
{
        int before_i=init_i, before_j=init_j;
        int after_i, after_j;
        char after;
        int shanon=0;
        //int shanon_i, shanon_j;
        int shanonMAX=0;
        int shanonMAX_i=0, shanonMAX_j=0;
        // printf("the piece name in genmove is %c \n", piece_name);

        //initial coord
        int shanon_init_i=init_i;
        int shanon_init_j=init_j;
        switch(piece_name)
        {
                case 'p'://for black pawn
                {
                        int check=0; //checking the (3) tiles on the row under, that are in diagonal/under the black pawn
                        for(int fin_i=init_i+1, fin_j=init_j-1; fin_j<=init_j+1; fin_j++)
                        {
                              check=pawn_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                              if(check==1)
                              {
                                        /*char result[3];
                                        decode_i_j_to_coord(fin_i, fin_j, result);
                                        printf("a possible move is %c%s why the under \n", piece_name, result);
                                        */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                        //the nrpieces_array vector has the following template
                                        // 0 1 2 3 4 5
                                        // P N B R Q K |white
                                        // p n b r q k |black

                                        //copying the coords and the pieces/space of the final poz
                                        after_i=fin_i;
                                        after_j=fin_j;
                                        after=board[after_i][after_j];

                                        //simulating move
                                        board[after_i][after_j]=piece_name;

                                        // calc nr of pieces after move
                                        nr_pieceW(board, coordarrayW, nrpieces_arrayW);
                                        nr_pieceB(board, coordarrayB, nrpieces_arrayB);

                                        //calculating move value
                                        shanon=200*(nrpieces_arrayB[5]-nrpieces_arrayW[5])+9*(nrpieces_arrayB[4]-nrpieces_arrayW[4])+
                                                5*(nrpieces_arrayB[3]-nrpieces_arrayW[3])+3*(nrpieces_arrayB[2]-nrpieces_arrayW[2]+
                                                nrpieces_arrayB[1]-nrpieces_arrayW[1])+1*(nrpieces_arrayB[0]-nrpieces_arrayW[0])-
                                        0.5*(doubled_pawnB(board, coordarrayB)-doubled_pawnW(board, coordarrayW)+
                                                blocked_pawnB(board, coordarrayB)-blocked_pawnW(board, coordarrayW)+
                                                isolated_pawnB(board, coordarrayB)-isolated_pawnW(board, coordarrayW))+
                                        0.1*(mobilityB(board, coordarrayB)-mobilityW(board, coordarrayW));
                                        if(shanon<=shanonMAX)
                                        {
                                                shanonMAX=shanon;
                                                shanonMAX_i=fin_i;
                                                shanonMAX_j=fin_j;
                                        }
                                        
                                        //reverting the board
                                        board[before_i][before_j]=piece_name;
                                        board[after_i][after_j]=after;
                              }  
                        }
                        break;
                }
                case 'P'://for white pawn
                {
                        int check=0; //checking the (3) tiles on the row above, that are in diagonal/above the wite pawn
                        for(int fin_i=init_i-1, fin_j=init_j-1; fin_j<=init_j+1; fin_j++)
                        {
                              check=pawn_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                              if(check==1)
                              {
                                        /*char result[3];
                                        decode_i_j_to_coord(fin_i, fin_j, result);
                                        printf("a possible move is %c%s why the under \n", piece_name, result);
                                        */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                        //the nrpieces_array vector has the following template
                                        // 0 1 2 3 4 5
                                        // P N B R Q K |white
                                        // p n b r q k |black

                                        //copying the coords and the pieces/space of the final poz
                                        after_i=fin_i;
                                        after_j=fin_j;
                                        after=board[after_i][after_j];

                                        //simulating move
                                        board[after_i][after_j]=piece_name;

                                        // calc nr of pieces after move
                                        nr_pieceW(board, coordarrayW, nrpieces_arrayW);
                                        nr_pieceB(board, coordarrayB, nrpieces_arrayB);

                                        //calculating move value
                                        shanon=200*(nrpieces_arrayB[5]-nrpieces_arrayW[5])+9*(nrpieces_arrayB[4]-nrpieces_arrayW[4])+
                                                5*(nrpieces_arrayB[3]-nrpieces_arrayW[3])+3*(nrpieces_arrayB[2]-nrpieces_arrayW[2]+
                                                nrpieces_arrayB[1]-nrpieces_arrayW[1])+1*(nrpieces_arrayB[0]-nrpieces_arrayW[0])-
                                        0.5*(doubled_pawnB(board, coordarrayB)-doubled_pawnW(board, coordarrayW)+
                                                blocked_pawnB(board, coordarrayB)-blocked_pawnW(board, coordarrayW)+
                                                isolated_pawnB(board, coordarrayB)-isolated_pawnW(board, coordarrayW))+
                                        0.1*(mobilityB(board, coordarrayB)-mobilityW(board, coordarrayW));
                                        if(shanon<=shanonMAX)
                                        {
                                                shanonMAX=shanon;
                                                shanonMAX_i=fin_i;
                                                shanonMAX_j=fin_j;
                                        }
                                        
                                        //reverting the board
                                        board[before_i][before_j]=piece_name;
                                        board[after_i][after_j]=after;
                              }  
                        }
                        break;
                }
                case 'r'://for black rook
                case 'R'://for white rook (both have the same conditions)
                {
                        int check=0;
                        for(int fin_i=0; fin_i<=7; fin_i++)
                        {
                                for(int fin_j=1; fin_j<=8; fin_j++)
                                {
                                        check=rook_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                                /*char result[3];
                                                decode_i_j_to_coord(fin_i, fin_j, result);
                                                printf("a possible move is %c%s why the under \n", piece_name, result);
                                                */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                                //the nrpieces_array vector has the following template
                                                // 0 1 2 3 4 5
                                                // P N B R Q K |white
                                                // p n b r q k |black

                                                //copying the coords and the pieces/space of the final poz
                                                after_i=fin_i;
                                                after_j=fin_j;
                                                after=board[after_i][after_j];

                                                //simulating move
                                                board[after_i][after_j]=piece_name;

                                                // calc nr of pieces after move
                                                nr_pieceW(board, coordarrayW, nrpieces_arrayW);
                                                nr_pieceB(board, coordarrayB, nrpieces_arrayB);

                                                //calculating move value
                                                shanon=200*(nrpieces_arrayB[5]-nrpieces_arrayW[5])+9*(nrpieces_arrayB[4]-nrpieces_arrayW[4])+
                                                        5*(nrpieces_arrayB[3]-nrpieces_arrayW[3])+3*(nrpieces_arrayB[2]-nrpieces_arrayW[2]+
                                                        nrpieces_arrayB[1]-nrpieces_arrayW[1])+1*(nrpieces_arrayB[0]-nrpieces_arrayW[0])-
                                                0.5*(doubled_pawnB(board, coordarrayB)-doubled_pawnW(board, coordarrayW)+
                                                        blocked_pawnB(board, coordarrayB)-blocked_pawnW(board, coordarrayW)+
                                                        isolated_pawnB(board, coordarrayB)-isolated_pawnW(board, coordarrayW))+
                                                0.1*(mobilityB(board, coordarrayB)-mobilityW(board, coordarrayW));
                                                if(shanon<=shanonMAX)
                                                {
                                                        shanonMAX=shanon;
                                                        shanonMAX_i=fin_i;
                                                        shanonMAX_j=fin_j;
                                                }
                                                
                                                //reverting the board
                                                board[before_i][before_j]=piece_name;
                                                board[after_i][after_j]=after;
                                        }
                                }
                        }
                        break;
                }
                case 'b'://for black bishop
                case 'B'://for white bisop (both)
                {
                        int check=0;
                        for(int fin_i=0; fin_i<=7; fin_i++)
                        {
                                for(int fin_j=1; fin_j<=8; fin_j++)
                                {
                                        check=bishop_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*char result[3];
                                        decode_i_j_to_coord(fin_i, fin_j, result);
                                        printf("a possible move is %c%s why the under \n", piece_name, result);
                                        */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                        //the nrpieces_array vector has the following template
                                        // 0 1 2 3 4 5
                                        // P N B R Q K |white
                                        // p n b r q k |black

                                        //copying the coords and the pieces/space of the final poz
                                        after_i=fin_i;
                                        after_j=fin_j;
                                        after=board[after_i][after_j];

                                        //simulating move
                                        board[after_i][after_j]=piece_name;

                                        // calc nr of pieces after move
                                        nr_pieceW(board, coordarrayW, nrpieces_arrayW);
                                        nr_pieceB(board, coordarrayB, nrpieces_arrayB);

                                        //calculating move value
                                        shanon=200*(nrpieces_arrayB[5]-nrpieces_arrayW[5])+9*(nrpieces_arrayB[4]-nrpieces_arrayW[4])+
                                                5*(nrpieces_arrayB[3]-nrpieces_arrayW[3])+3*(nrpieces_arrayB[2]-nrpieces_arrayW[2]+
                                                nrpieces_arrayB[1]-nrpieces_arrayW[1])+1*(nrpieces_arrayB[0]-nrpieces_arrayW[0])-
                                        0.5*(doubled_pawnB(board, coordarrayB)-doubled_pawnW(board, coordarrayW)+
                                                blocked_pawnB(board, coordarrayB)-blocked_pawnW(board, coordarrayW)+
                                                isolated_pawnB(board, coordarrayB)-isolated_pawnW(board, coordarrayW))+
                                        0.1*(mobilityB(board, coordarrayB)-mobilityW(board, coordarrayW));
                                        if(shanon<=shanonMAX)
                                        {
                                                shanonMAX=shanon;
                                                shanonMAX_i=fin_i;
                                                shanonMAX_j=fin_j;
                                        }
                                        
                                        //reverting the board
                                        board[before_i][before_j]=piece_name;
                                        board[after_i][after_j]=after;

                                        }
                                }
                        }
                        break;
                }
                case 'q'://for white queen
                case 'Q'://for black queen
                {
                        int check=0;
                        for(int fin_i=0; fin_i<=7; fin_i++)
                        {
                                for(int fin_j=1; fin_j<=8; fin_j++)
                                {
                                        check=queen_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                        /*char result[3];
                                        decode_i_j_to_coord(fin_i, fin_j, result);
                                        printf("a possible move is %c%s why the under \n", piece_name, result);
                                        */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                        //the nrpieces_array vector has the following template
                                        // 0 1 2 3 4 5
                                        // P N B R Q K |white
                                        // p n b r q k |black

                                        //copying the coords and the pieces/space of the final poz
                                        after_i=fin_i;
                                        after_j=fin_j;
                                        after=board[after_i][after_j];

                                        //simulating move
                                        board[after_i][after_j]=piece_name;

                                        // calc nr of pieces after move
                                        nr_pieceW(board, coordarrayW, nrpieces_arrayW);
                                        nr_pieceB(board, coordarrayB, nrpieces_arrayB);

                                        //calculating move value
                                        shanon=200*(nrpieces_arrayB[5]-nrpieces_arrayW[5])+9*(nrpieces_arrayB[4]-nrpieces_arrayW[4])+
                                                5*(nrpieces_arrayB[3]-nrpieces_arrayW[3])+3*(nrpieces_arrayB[2]-nrpieces_arrayW[2]+
                                                nrpieces_arrayB[1]-nrpieces_arrayW[1])+1*(nrpieces_arrayB[0]-nrpieces_arrayW[0])-
                                        0.5*(doubled_pawnB(board, coordarrayB)-doubled_pawnW(board, coordarrayW)+
                                                blocked_pawnB(board, coordarrayB)-blocked_pawnW(board, coordarrayW)+
                                                isolated_pawnB(board, coordarrayB)-isolated_pawnW(board, coordarrayW))+
                                        0.1*(mobilityB(board, coordarrayB)-mobilityW(board, coordarrayW));
                                        if(shanon<=shanonMAX)
                                        {
                                                shanonMAX=shanon;
                                                shanonMAX_i=fin_i;
                                                shanonMAX_j=fin_j;
                                        }
                                        
                                        //reverting the board
                                        board[before_i][before_j]=piece_name;
                                        board[after_i][after_j]=after;
                                        }
                                }
                        }
                        break;
                }
                case 'n':// for black knight
                case 'N':// for white knight
                {
                        int check=0;
                        for(int fin_i=init_i-2; fin_i<=init_i+2; fin_i++)
                        {
                                for(int fin_j=init_j-2; fin_j<=init_j+2; fin_j++)
                                {
                                        check=knight_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*char result[3];
                                        decode_i_j_to_coord(fin_i, fin_j, result);
                                        printf("a possible move is %c%s why the under \n", piece_name, result);
                                        */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                        //the nrpieces_array vector has the following template
                                        // 0 1 2 3 4 5
                                        // P N B R Q K |white
                                        // p n b r q k |black

                                        //copying the coords and the pieces/space of the final poz
                                        after_i=fin_i;
                                        after_j=fin_j;
                                        after=board[after_i][after_j];

                                        //simulating move
                                        board[after_i][after_j]=piece_name;

                                        // calc nr of pieces after move
                                        nr_pieceW(board, coordarrayW, nrpieces_arrayW);
                                        nr_pieceB(board, coordarrayB, nrpieces_arrayB);

                                        //calculating move value
                                        shanon=200*(nrpieces_arrayB[5]-nrpieces_arrayW[5])+9*(nrpieces_arrayB[4]-nrpieces_arrayW[4])+
                                                5*(nrpieces_arrayB[3]-nrpieces_arrayW[3])+3*(nrpieces_arrayB[2]-nrpieces_arrayW[2]+
                                                nrpieces_arrayB[1]-nrpieces_arrayW[1])+1*(nrpieces_arrayB[0]-nrpieces_arrayW[0])-
                                        0.5*(doubled_pawnB(board, coordarrayB)-doubled_pawnW(board, coordarrayW)+
                                                blocked_pawnB(board, coordarrayB)-blocked_pawnW(board, coordarrayW)+
                                                isolated_pawnB(board, coordarrayB)-isolated_pawnW(board, coordarrayW))+
                                        0.1*(mobilityB(board, coordarrayB)-mobilityW(board, coordarrayW));
                                        if(shanon<=shanonMAX)
                                        {
                                                shanonMAX=shanon;
                                                shanonMAX_i=fin_i;
                                                shanonMAX_j=fin_j;
                                        }
                                        
                                        //reverting the board
                                        board[before_i][before_j]=piece_name;
                                        board[after_i][after_j]=after;
                                        }  
                                }
                        }
                        break;
                }
                case 'k':
                case 'K':
                {
                        int check=0;
                        for(int fin_i=init_i-1; fin_i<=init_i+1; fin_i++)
                        {
                                for(int fin_j=init_j-1; fin_j<=init_j+1; fin_j++)
                                {
                                        check=king_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        if(check==1)
                                        {
                                          /*char result[3];
                                        decode_i_j_to_coord(fin_i, fin_j, result);
                                        printf("a possible move is %c%s why the under \n", piece_name, result);
                                        */ //THIS IS THE TEST FOR GEN IN CONSOLE

                                        //the nrpieces_array vector has the following template
                                        // 0 1 2 3 4 5
                                        // P N B R Q K |white
                                        // p n b r q k |black

                                        //copying the coords and the pieces/space of the final poz
                                        after_i=fin_i;
                                        after_j=fin_j;
                                        after=board[after_i][after_j];

                                        //simulating move
                                        board[after_i][after_j]=piece_name;

                                        // calc nr of pieces after move
                                        nr_pieceW(board, coordarrayW, nrpieces_arrayW);
                                        nr_pieceB(board, coordarrayB, nrpieces_arrayB);

                                        //calculating move value
                                        shanon=200*(nrpieces_arrayB[5]-nrpieces_arrayW[5])+9*(nrpieces_arrayB[4]-nrpieces_arrayW[4])+
                                                5*(nrpieces_arrayB[3]-nrpieces_arrayW[3])+3*(nrpieces_arrayB[2]-nrpieces_arrayW[2]+
                                                nrpieces_arrayB[1]-nrpieces_arrayW[1])+1*(nrpieces_arrayB[0]-nrpieces_arrayW[0])-
                                        0.5*(doubled_pawnB(board, coordarrayB)-doubled_pawnW(board, coordarrayW)+
                                                blocked_pawnB(board, coordarrayB)-blocked_pawnW(board, coordarrayW)+
                                                isolated_pawnB(board, coordarrayB)-isolated_pawnW(board, coordarrayW))+
                                        0.1*(mobilityB(board, coordarrayB)-mobilityW(board, coordarrayW));
                                        if(shanon<=shanonMAX)
                                        {
                                                shanonMAX=shanon;
                                                shanonMAX_i=fin_i;
                                                shanonMAX_j=fin_j;
                                        }
                                        
                                        //reverting the board
                                        board[before_i][before_j]=piece_name;
                                        board[after_i][after_j]=after;
                                        }      
                                }
                        }
                        break;
                }
                
                

        }
        //puting in the best moves array the piece name, coords and the value of the move
                int name=0;
                int i_coord=1;
                int j_coord=2;
                int value=3;
                *(best_moves_arrayB+k)=piece_name;
                *(best_moves_arrayB+16+k)=shanonMAX_i;
                *(best_moves_arrayB+32+k)=shanonMAX_j;
                *(best_moves_arrayB+48+k)=shanonMAX;
                *(best_moves_arrayB+64+k)=shanon_init_i;
                *(best_moves_arrayB+80+k)=shanon_init_j;

               // printf("in gen move name %c, i=%i, j=%i, v=%i \n ", piece_name, shanonMAX_i, shanonMAX_j, shanonMAX);

}


int best_movesB(char* board[][DIM], int* coordarrayB, int* coordarrayW,
                int* nrpieces_arrayB, int* nrpieces_arrayW, int *best_moves_arrayB)
{

        int foundB=search_boardB(board, coordarrayB);

        int name=0;
        int i_coord=1;
        int j_coord=2;

        int best_move_index;
        int best_move_name;
        int best_move=0;
        int best_move_i;
        int best_move_j;
        
        int before_best_move_i;
        int before_best_move_j;

        for(int k=0; k<foundB; k++)
        {
                gen_move_S(board, *(coordarrayB+16+k), *(coordarrayB+32+k), *(coordarrayB+k), coordarrayW, coordarrayB,
                           nrpieces_arrayW, nrpieces_arrayB, best_moves_arrayB, k);

        }

        for(int i=0; i<foundB; i++)
        {
                if(*(best_moves_arrayB+48+i)<best_move)
                {
                        best_move=*(best_moves_arrayB+48+i);
                        best_move_i=*(best_moves_arrayB+16+i);
                        best_move_j=*(best_moves_arrayB+32+i);
                        best_move_name=*(best_moves_arrayB+i);
                        best_move_index=i;

                        before_best_move_i=*(best_moves_arrayB+64+i);
                        before_best_move_j=*(best_moves_arrayB+80+i);
                }
        }
        // printf("the best move is %i, I=%i, J=%i, name=%c \n", *(best_moves_arrayB+48+best_move_i), best_move_i, best_move_j, best_move_name);
        /*
        for( int i=0; i<foundB; i++)
        {
                printf("the shanon value %i, is = %i \n", *(best_moves_arrayB+i), *(best_moves_arrayB+48+i));  
        }
        */

        board[best_move_i][best_move_j]=best_move_name;
        board[before_best_move_i][before_best_move_j]=' ';
        print_board(board);
        return best_move_index;
}

void move_pvp(char* board[][DIM], int player_turn)
{
      //the accepted format is INITIAL_POZ-FINAL_POZ
        // INITIAL_POZ and/or FINAL_POZ being PIECE_NAME_LETTERCOORD_NUMBERCOORD
        //EXAMPLE PA2-PA3
        char init[4];
        char final[4];
        do
        {
                if(check_mate(board, coordarrayW, coordarrayB)==1)
                {
                        printf("black wins!");
                        exit(0);
                }
                else if(check_mate(board, coordarrayW, coordarrayB)==2)
                {
                        printf("white wins!");
                        exit(0);
                }

                if(player_turn==1)
                {
                        printf("white turn\n");
                }
                else
                {
                        printf("black turn\n");
                }
                
                
                
                scanf("%3s-%3s", init, final);
                
                if(strcmp(init, "st")==0) exit(0);
                
                //printf("%s goes to %s \n", init, final);
                
                int init_i, init_j, fin_i, fin_j;
                
                int check=0;
                
                char piece_name;
                /*piece_name=init[0];
                
                init_j=init[1]-'A'+1;
                fin_j=final[1]-'A'+1;
                
                init_i=abs(init[2]-'8');
                fin_i=abs(final[2]-'8');
                */
                
                if(strcmp(init, "sav")==0)
                {
                        save_game(board, player_turn, saved_game_state, fin);
                        printf("game saved");
                        exit(0);
                }
                else
                {
                        printf("%s goes to %s \n", init, final);
                        piece_name=init[0];
                        init_j=init[1]-'A'+1;
                        fin_j=final[1]-'A'+1;
                
                        init_i=abs(init[2]-'8');
                        fin_i=abs(final[2]-'8');

                        if((player_turn==1 && islower(piece_name)) || (player_turn==2 && isupper(piece_name)))
                        {
                                printf("moved the wrong colour\n");
                        }
                        else
                        {              
                                if(piece_name=='p' || piece_name=='P')
                                {
                                        check=pawn_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                }
                                else
                                {
                                        if(piece_name=='r' || piece_name=='R')
                                        {
                                                check=rook_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                        }
                                        else
                                        {
                                        if(piece_name=='k' || piece_name=='K')
                                                {
                                                        check=king_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                                }
                                                else
                                                {
                                                        if(piece_name=='n' || piece_name=='N')
                                                        {
                                                                check=knight_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                                        }
                                                        else
                                                        {
                                                                if(piece_name=='b' || piece_name=='B')
                                                                {
                                                                        check=bishop_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                                                }
                                                                else
                                                                {
                                                                        if(piece_name=='q' || piece_name=='Q')
                                                                        {
                                                                                check=queen_check(init_i, init_j, fin_i, fin_j, piece_name, board);
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                        
                        if(check)
                        {
                                board[fin_i][fin_j]=board[init_i][init_j];
                                board[init_i][init_j]=' ';
                                print_board(board);

                        // printf("the mob of white is %i and the mob of black is %i \n", mobilityW(board, coordarrayW), mobilityB(board, coordarrayB));
                                //printf("isolated white %i, isolated black %i \n", isolated_pawnW(board, coordarrayW), isolated_pawnB(board, coordarrayB));
                        // printf("blocked white %i, blocked black %i \n", blocked_pawnW(board, coordarrayW), blocked_pawnB(board, coordarrayB));
                        // printf("doubled white %i, doubled black %i \n", doubled_pawnW(board, coordarrayW), doubled_pawnB(board, coordarrayB));

                        


                                //best_movesB(board, coordarrayB, coordarrayW, nrpieces_arrayB, nrpieces_arrayW, best_moves_arrayB);

                                if(player_turn==1)
                                {
                                        player_turn=2;
                                }
                                else
                                {
                                        if(player_turn==1)
                                        {
                                                player_turn=2;
                                        }
                                }
                        }
                        else
                        {
                                printf("impossible move \n");
                        }
                }

        }while(strcmp(init, "st")!=0);
        /*{
                move_pvp(board, player_turn);
        }  */
}

int check_check(char* board[][DIM], int *coordarrayW, int* coordarrayB)
{
        int iw, jw;
        int ib, jb;

        int check=0;
        int wfound=search_boardW(board, coordarrayW), bfound=search_boardB(board, coordarrayB);

        //white king check
        for(int i=0; i<wfound && check==0; i++)
        {
                if((*(coordarrayW+i))=='K')
                {
                        iw=*(coordarrayW+i+16);
                        jw=*(coordarrayW+i+32);
                        for(int j=0; j<bfound; j++)
                        {
                                int piece_name=*(coordarrayB+j);
                                int init_i=*(coordarrayB+j+16);
                                int init_j=*(coordarrayB+j+32);
                                switch(piece_name)
                                {
                                        case 'p':
                                        {
                                                check=pawn_check(init_i, init_j, iw, jw, piece_name, board);
                                                return check;
                                                break;
                                        }
                                        case 'r':
                                        {
                                                check=rook_check(init_i, init_j, iw, jw, piece_name, board);
                                                return check;
                                                break;
                                        }
                                        case 'b':
                                        {
                                                check=bishop_check(init_i, init_j, iw, jw, piece_name, board);
                                                return check;
                                                break;
                                        }
                                        case 'k':
                                        {
                                                check=knight_check(init_i, init_j, iw, jw, piece_name, board);
                                                return check;
                                                break;
                                        }
                                        case 'q':
                                        {
                                                check=queen_check(init_i, init_j, iw, jw, piece_name, board);
                                                return check;
                                                break;
                                        }
                                }
                        }
                        
                }
        }

        if(check==1) return 1;
        else
        {
                //black king check
                for(int i=0; i<=bfound && check==0; i++)
                {
                        if((*(coordarrayB+i))=='k')
                        {
                                ib=*(coordarrayB+i+16);
                                jb=*(coordarrayB+i+32);
                                for(int j=0; j<wfound; j++)
                                {
                                        int piece_name=*(coordarrayW+j);
                                        int init_i=*(coordarrayW+j+16);
                                        int init_j=*(coordarrayW+j+32);
                                        switch(piece_name)
                                        {
                                                case 'P':
                                                {
                                                        check=pawn_check(init_i, init_j, iw, jw, piece_name, board);
                                                        return check;
                                                        break;
                                                }
                                                case 'R':
                                                {
                                                        check=rook_check(init_i, init_j, iw, jw, piece_name, board);
                                                        return check;
                                                        break;
                                                }
                                                case 'B':
                                                {
                                                        check=bishop_check(init_i, init_j, iw, jw, piece_name, board);
                                                        return check;
                                                        break;
                                                }
                                                case 'K':
                                                {
                                                        check=knight_check(init_i, init_j, iw, jw, piece_name, board);
                                                        return check;
                                                        break;
                                                }
                                                case 'Q':
                                                {
                                                        check=queen_check(init_i, init_j, iw, jw, piece_name, board);
                                                        return check;
                                                        break;
                                                }
                                        }
                                }  
                        }
                }
                if(check==1) return 2;
                else
                {
                        if(check==0) return 3;
                }
        }
}

int check_mate(char* board[][DIM], int* coordarrayW, int* coordarrayB)
{
        int foundwK=0;
        int foundbk=0;
        int foundw=search_boardW(board, coordarrayW);
        int foundb=search_boardB(board, coordarrayB);

        for(int i=0; i<=foundw && foundwK==0; i++)
        {
                if(*(coordarrayW+i)=='K') foundwK=1;
        }

        for(int i=0; i<=foundb && foundbk==0; i++)
        {
                if(*(coordarrayB+i)=='k') foundbk=1;
        }

        if(foundwK==0) return 1;
        else
        {
                if(foundbk==0) return 2;
                else
                {
                        return 0;
                }
        }
        
}

int save_game(char* board[][DIM], int player_turn, int saved_game_state, FILE* fin)
{       // 0 no saved game 1/2(player turn) or 12(game state) for pvp and 3 for pvc

        if(player_turn==3)
        {
                saved_game_state=3;
        }
        else
        {
                saved_game_state=12;
        }
        
        fprintf(fin, "%i\n", player_turn);
        fprintf(fin, "%i\n", saved_game_state);
        for(int i=0; i<9; i++)
        {
               for(int j=0; j<9; j++)
               {
                        if(board[i][j]==' ')
                        {
                                fprintf(fin, "s ");
                        }
                        else
                        {
                                fprintf(fin, "%c ", board[i][j]);
                        }
               }
               fprintf(fin, "\n");
        }
        return 0;
}
