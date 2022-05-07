/* 
 * name:        万金易
 * studentID:   519021911049
 * loginID:     ics519021911049
 * trans.c - Matrix transpose B = A^T
 *
 * s = 5, E = 1, b = 5
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{    
    int big_r;
    int big_c;
    int row;
    int col;
    
    int buf0;
    int buf1;
    int buf2;
    int buf3;
    int buf4;
    int buf5;
    int buf6;
    int buf7;
    
    if(M == 32 && N == 32){
      for(big_r = 0;big_r < 4; ++ big_r){
        for(big_c = 0; big_c < 4; ++ big_c){
          for(row = big_r<<3; row < (big_r+1) << 3 ; ++ row){
            if(big_r != big_c){
              for(col = big_c << 3; col < (big_c + 1) << 3 ; ++ col){
                  B[col][row] = A[row][col];
              }
            }else{
                col = big_c << 3;
                buf0 = A[row][col];
                buf1 = A[row][col + 1];
                buf2 = A[row][col + 2];
                buf3 = A[row][col + 3];
                buf4 = A[row][col + 4];
                buf5 = A[row][col + 5];
                buf6 = A[row][col + 6];
                buf7 = A[row][col + 7];
                B[col][row] = buf0;
                B[col + 1][row] = buf1;
                B[col + 2][row] = buf2;
                B[col + 3][row] = buf3;
                B[col + 4][row] = buf4;
                B[col + 5][row] = buf5;
                B[col + 6][row] = buf6;
                B[col + 7][row] = buf7;
            }
          }
        }
      }
      
    }else if (N == 67 && M == 61){
      //17*4
      for(big_r = 0;big_r < 4; ++ big_r){
        for(big_c = 0; big_c < 16; ++ big_c){
          for(row = big_r * 17; row < (big_r+1) * 17 && row < N; ++ row){
              for(col = big_c * 4; col < (big_c + 1) * 4 && col < M; ++ col){
                  B[col][row] = A[row][col];
              }
          }
        }
      }
    }else if (M == 64 && N == 64){
      //8*8套4*4
      for(big_r = 0; big_r < 8; ++big_r){
        for(big_c = 0; big_c < 8; ++ big_c){
          //将A上移动至B上
          col = big_c << 3;
          for(row = big_r << 3; row < (big_r << 3) + 4; ++ row){
            buf0 = A[row][col + 0];
            buf1 = A[row][col + 1];
            buf2 = A[row][col + 2];
            buf3 = A[row][col + 3];
            buf4 = A[row][col + 4];
            buf5 = A[row][col + 5];
            buf6 = A[row][col + 6];
            buf7 = A[row][col + 7];
            //A左上翻转至B左上
            B[col + 0][row] = buf0;
            B[col + 1][row] = buf1;
            B[col + 2][row] = buf2;
            B[col + 3][row] = buf3;
            
            //A右上 翻折+平移到 B 右上
            B[col + 0][row + 4] = buf4;
            B[col + 1][row + 4] = buf5;
            B[col + 2][row + 4] = buf6;
            B[col + 3][row + 4] = buf7;
          }
          row = big_r << 3;
          for(col = (big_c << 3) + 4; col < (big_c + 1) << 3; ++ col){
            buf0 = B[col - 4][row + 4];
            buf1 = B[col - 4][row + 5];
            buf2 = B[col - 4][row + 6];
            buf3 = B[col - 4][row + 7];
            
            buf4 = A[row + 4][col - 4];
            buf5 = A[row + 5][col - 4];
            buf6 = A[row + 6][col - 4];
            buf7 = A[row + 7][col - 4];
            
            B[col - 4][row + 4] = buf4;
            B[col - 4][row + 5] = buf5;
            B[col - 4][row + 6] = buf6;
            B[col - 4][row + 7] = buf7;
            
            B[col][row + 0] = buf0;
            B[col][row + 1] = buf1;
            B[col][row + 2] = buf2;
            B[col][row + 3] = buf3;
          }

          
          
          col = big_c << 3;
          for(row = (big_r << 3) + 4; row < (big_r + 1) << 3; ++ row){
            buf0 = A[row][col + 4];
            buf1 = A[row][col + 5];
            buf2 = A[row][col + 6];
            buf3 = A[row][col + 7];
            
            B[col + 4][row] = buf0;
            B[col + 5][row] = buf1;
            B[col + 6][row] = buf2;
            B[col + 7][row] = buf3;
            
          }
          
        }
      }
      
    }else{//otherwise 8 * 4分块
      for(big_r = 0;big_r < (N + 7) << 3; ++ big_r){
        for(big_c = 0; big_c < (M + 7) << 2; ++ big_c){
          for(row = big_r<<3; row < (big_r+1) << 3 && row < N; ++ row){
            for(col = big_c << 2; col < (big_c + 1) << 2 && col < M; ++ col){
              B[col][row] = A[row][col];
            }
          }
        }
      }
    }
    
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}
/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

