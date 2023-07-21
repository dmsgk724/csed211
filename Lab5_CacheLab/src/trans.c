/* 
박은하 20200445  dmsgk724 
 * trans.c - Matrix transpose B = A^T
 *
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
    int j1,i1,j,i;
        int temp, d=-1;
    if ((M == 32) && (N == 32))
    {
        for (i = 0; i < 32; i += 8)
        {
            for (j = 0; j < 32; j += 8)
            {
                for (i1 = i; i1 < i + 8; i1++) {
                    for (j1 = j; j1 < j + 8; j1++)
                    {

                        if (j1 != i1) {
                            B[j1][i1] = A[i1][j1];
                        }
                        else
                        {
                            temp = A[i1][j1];
                            d = i1;
                         
                        }
                    }
                    if(i1==d)
                    {    B[d][d] = temp;}
                    d=-1;
                }

            }
        }
    }
   else if ((M == 64) && (N == 64))
    {
        int v0, v1, v2, v3, v4, v5, v6, v7;
        int k = 0;
        for (int i = 0; i < 64; i += 8)
        {
            for (int j = 0; j < 64; j += 8)
            {
                
                 for (k = 0; k < 4; k++)
                    {
                        v0 = A[k+i][j + 0];
                        v1 = A[k+i][j + 1];
                        v2 = A[k+i][j + 2];
                        v3 = A[k+i][j + 3];
                        v4 = A[k+i][j + 4];
                        v5 = A[k+i][j + 5];
                        v6 = A[k+i][j + 6];
                        v7 = A[k+i][j + 7];

                        B[j][k+i] = v0;
                        B[j + 1][k+i] = v1;
                        B[j + 2][k+i] = v2;
                        B[j + 3][k+i] = v3;   
                        B[j][k+i + 4] = v7;
                        B[j+1][k+i + 4] = v6;
                        B[j+2][k+i + 4] = v5;
                        B[j+3][k+i + 4] = v4;                        

                 }
                for (k = 0; k < 4; k++)
                {
                  
                    v0 = A[i+4][j + 3-k];
                    v1 = A[i+5][j + 3-k];
                    v2 = A[i+6][j + 3-k];
                    v3 = A[i+7][j + 3-k];
                    v4 = A[i+4][j + 4+k];
                    v5 = A[i+5][j + 4+k];
                    v6 = A[i+6][j + 4+k];
                    v7 =A[i+7][j + 4+k];
                    B[j+4+k][i+0] = B[j+3-k][i+4];
                    B[j+4+k][i+1] = B[j+3-k][i+5];
                    B[j+4+k][i+2] = B[j+3-k][i+6];
                    B[j+4+k][i+3] = B[j+3-k][i+7];
                    B[j+3-k][i+4]=v0;
                    B[j+3-k][i+5]=v1;
                    B[j+3-k][i+6]=v2;
                    B[j+3-k][i+7]=v3;
                    B[j+4+k][i+4]=v4;
                     B[j+4+k][i+5]=v5;
                    B[j+4+k][i+6]=v6;
                     B[j+4+k][i+7]=v7;

                }
                 
            }
               
        }
    }
    else if (M!=N)
    {
       

        for (i = 0; i < N; i += 16)
        {
            for (j = 0; j < M; j += 16)
            {
              for ( i1 = i; (i1 < i + 16)&&(i1<N); i1++) 
              {
                    for (j1 = j; (j1 < j + 16)&&(j1<M); j1++)
                    {

                        if (j1 != i1) {
                            B[j1][i1] = A[i1][j1];
                        }
                        else
                        {
                            temp = A[i1][j1];
                            d = i1;
                          

                        }
                    }
                if(i1==d)
                        {B[d][d] = temp;}
                        d=-1;
                    
            }
            }
        }
        
    }


}/* 
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

