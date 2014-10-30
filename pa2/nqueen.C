MAIN_ENV

/* Temporary global variables */
int nSol = 0;
int prof = 0;
char* bestSolution;
int bestProf = 0;

/* A function to print solutions. */
void printSolution(char** board, int n)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf(" %d ", board[i][j]);
        printf("\n");
    }
}

/* A function to print the total number of solutions, the solution with
 * the largest profit, and its corresponding profit. */
void printBestSolution(char* bestSolution, int n)
{
    int i, j;
    printf("Total # of Solutions: %d\n", nSol);
    printf("The Largest Profit: %d\n", bestProf);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf(" %d ", bestSolution[i*n+j]);
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
void solveNQ(char** board, int n, int col)
{
    // Base case: all queens are placed
    if (col >= n)
    {
        nSol++;

        // Check the profit and change the bestSolution accordingly
        if (prof > bestProf)
        {
            int r, c;
            for(r = 0; r < n; r++)
                for (c = 0; c < n; c++)
                    bestSolution[r*n+c] = board[r][c];
            bestProf = prof;
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
            prof += (i-col > 0) ? (i-col) : (col-i);
            solveNQ(board, n, col+1);
            board[i][col] = 0;
            // Remove profit
            prof -= (i-col > 0) ? (i-col) : (col-i);
        }
}

int main(int argc, char** argv)
{
    MAIN_INITENV
    if (argc != 2)
    {
        printf("Usage: nqueen <size>\nAborting...\n");
        exit(0);
    }

    // Generate the chessboard and initialize
    int n = atoi(argv[1]);
    char** chessboard = (char**)G_MALLOC(n*sizeof(char*))
    int i, j;
    for (i = 0; i < n; i++)
    {
        chessboard[i] = (char*)G_MALLOC(n*sizeof(char))
        for (j = 0; j < n; j++)
            chessboard[i][j] = 0;
    }

    // Generate the best solution board storage
    bestSolution = (char*)G_MALLOC(n*n*sizeof(char))
    
    // Place the queens
    solveNQ(chessboard, n, 0);

    // Print best solution
    printBestSolution(bestSolution, n);

    // Free the chessboard
    for (i = 0; i < n; i++)
        G_FREE(chessboard[i], n*sizeof(char))
    G_FREE(chessboard, n*sizeof(char*))

    // Free the bestSolution
    G_FREE(bestSolution, n*n*sizeof(char))
}
