MAIN_ENV

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
char solveNQ(char** board, int n, int col)
{
    // Base case: if all queens are placed then return true
    if (col >= n)
        return -1;

    // Consider this column and try placing the queen in all
    // columns one by one
    int i;
    for (i = 0; i < n; i++)
    {
        if (isSafe(board, n, i, col))
        {
            board[i][col] = 1;

            if (solveNQ(board, n, col+1))
                return -1;

            board[i][col] = 0;
        }
    }

    // If the queen cannot be placed in any rows in this column,
    // return false
    return 0;
}

/* This function solves the NQ problem by backtracking. It returns 0 
 * no solutions are found and -1 if at least one solution exists. It also
 * prints out one of the feasible solutions.*/
char nqueen(char** board, int n)
{
    if (!solveNQ(board, n, 0))
    {
        printf("Solution does not exist!\n");
        return 0;
    }

    printSolution(board, n);
    return -1;

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
    
    // Place the queens and print out the solution
    nqueen(chessboard, n);

    // Free the chessboard
    for (i = 0; i < n; i++)
        G_FREE(chessboard[i], n*sizeof(char))
    G_FREE(chessboard, n*sizeof(char*))
}
