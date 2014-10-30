MAIN_ENV

/* Temporary global variables */
int n;
int p;
int* nSol;
int* prof;
int* bestProf;
char** bestSolution;

/* A function to print the total number of solutions, the solution with
 * the largest profit, and its corresponding profit. */
void printBestSolution(void)
{
    int i, j;
    int finalProf = 0;
    int idx;
    for (i = 0; i < p; i++)
    {
        if (bestProf[i] > finalProf)
        {
            finalProf = bestProf[i];
            idx = i;
        }
    }

    // Print the best solution
    int finalNSol = 0;
    for (i = 0; i < p; i++)
        finalNSol += nSol[i];

    printf("Total # of Solutions: %d\n", finalNSol);
    printf("The Largest Profit: %d\n", finalProf);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf(" %d ", bestSolution[idx][i*n+j]);
        printf("\n");
    }
}

/* A function to check if a queen can be placed on board[row][col].
 * Note that this function is called when col queens are already 
 * placed in columns from 0 to col-1. So we need to check only left 
 * side for attacking queens. */
char isSafe(char** board, int n, int row, int col)
{
    int i, j;

    // Check this row on left side
    for (i = 0; i < col; i++)
    {
        if (board[row][i])
            return 0;
    }

    // Check upper diagonal on the left side
    for (i = row, j = col; i >= 0 && j >= 0; i--, j--)
    {
        if (board[i][j])
            return 0;
    }

    // Check lower diagonal on the left side
    for (i = row, j = col; i < n && j >= 0; i++, j--)
    {
        if (board[i][j])
            return 0;
    }

    return -1;
}

/* Recursive function to solve n quenns problem. */
void solveNQ(char** board, int n, int col, int pid)
{
    // Base case: all queens are placed
    if (col >= n)
    {
        nSol[pid]++;

        // Check the profit and change the bestSolution accordingly
        if (prof[pid] > bestProf[pid])
        {
            int r, c;
            for(r = 0; r < n; r++)
                for (c = 0; c < n; c++)
                    bestSolution[pid][r*n+c] = board[r][c];
            bestProf[pid] = prof[pid];
        }

        return;
    }

    // Consider this column and try placing the queen in all
    // rows one by one
    int i;
    for (i = 0; i < n; i++)
        if (isSafe(board, n, i, col))
        {
            board[i][col] = 1;
            // Add profit
            prof[pid] += (i-col > 0) ? (i-col) : (col-i);
            solveNQ(board, n, col+1, pid);
            board[i][col] = 0;
            // Remove profit
            prof[pid] -= (i-col > 0) ? (i-col) : (col-i);
        }
}

/* A function used to split work among the threads. */
void solveNQWrapper(void)
{
    int pid;
    GET_PID(pid);

    // Generate the chessboard and initialize
    char** board = (char**)G_MALLOC(n*sizeof(char*))
    int i, j;
    for (i = 0; i < n; i++)
    {
        board[i] = (char*)G_MALLOC(n*sizeof(char))
        for (j = 0; j < n; j++)
            board[i][j] = 0;
    }
    
    for (i = pid; i < n; i += p)
    {
        board[i][0] = 1;
        prof[pid] += i;
        solveNQ(board, n, 1, pid);
        board[i][0] = 0;
        prof[pid] -= i;
    }

    // Free the chessboard
    for (i = 0; i < n; i++)
        G_FREE(board[i], n*sizeof(char))
    G_FREE(board, n*sizeof(char*))
}

int main(int argc, char** argv)
{
    MAIN_INITENV

    unsigned int t1, t2;
    CLOCK(t1)
    if (argc != 3)
    {
        printf("Usage: nqueen <size> <nProc>\nAborting...\n");
        exit(0);
    }

    n = atoi(argv[1]);
    p = atoi(argv[2]);

    if (n < 2)
    {
        printf("Are you kidding?\n");
        exit(0);
    }

    int i;

    // Generate the best solution board storage
    bestSolution = (char**)G_MALLOC(p*sizeof(char*))
    for (i = 0; i < p; i++)
        bestSolution[i] = (char*)G_MALLOC(n*n*sizeof(char))

    // Generate the array of profits and nSol
    prof = (int*)G_MALLOC(p*sizeof(int))
    for (i = 0; i < p; i++)
        prof[i] = 0;

    nSol = (int*)G_MALLOC(p*sizeof(int))
    for (i = 0; i < p; i++)
        nSol[i] = 0;

    bestProf = (int*)G_MALLOC(p*sizeof(int))
    for (i = 0; i < p; i++)
        bestProf[i] = 0;

    CLOCK(t2)
    printf("Intialization Time: %u us\n", t2-t1);

    // Place the queens
    for (i = 0; i < p-1; i++)
        CREATE(solveNQWrapper)
    CLOCK(t1)
    solveNQWrapper();
    WAIT_FOR_END(p-1)
    CLOCK(t2)
    printf("Computation  Time: %u us\n", t2-t1);

    // Print best solution
    CLOCK(t1)
    printBestSolution();
    CLOCK(t2)
    printf("Finishing Time: %u us\n", t2-t1);

    // Free the bestSolution
    for (i = 0; i < p; i++)
        G_FREE(bestSolution[i], n*n*sizeof(char))
    G_FREE(bestSolution, p*sizeof(char*))

    // Free the prof and nSol
    G_FREE(prof, p*sizeof(int))
    G_FREE(nSol, p*sizeof(int))
    G_FREE(bestProf, p*sizeof(int))

}
