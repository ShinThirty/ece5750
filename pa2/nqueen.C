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
char isSafe(char** board, int row, int col, int n)
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

void nqueen()
{

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
    char** chessboard = (char**)G_MALLOC(n*sizeof(char*));
    int i, j;
    for (i = 0; i < n; i++)
    {
        chessboard[i] = (char*)G_MALLOC(n*sizeof(char));
        for (j = 0; j < n; j++)
            chessboard[i][j] = 0;
    }
    
    // Place the queens and print out the solution
    nqueen();
}
