MAIN_ENV

typedef struct {
  double **a;
  double *b;
  int n,p;
  char *pse;
  
  // 1D array mapped from a
  double * a_prime;

} GM;

GM *gm;

void pbksb(void) {
  register int i,j;
  register double sum;
  int pid;
  double *b;
  int n,p;
  GET_PID(pid);
  b = gm->b;
  n = gm->n;
  p = gm->p;
  double *a_prime;
  a_prime = gm->a_prime;

  //--------------------------------------------------
  // Parallelize the consecutive substract operations
  //--------------------------------------------------
  for (i = n-p+pid; i >= pid; i -= p)
  {
    sum = b[i];
    for (j = n-1; j > i; j--)
    {
      WAITPAUSE(gm->pse[j])
      sum -= a_prime[(n-i) * (n-1-i)/2 + (n-1-j)] * b[j];
    }

    b[i] = sum / a_prime[(n-i) * (n-1-i)/2 + (n-1-i)];
    SETPAUSE(gm->pse[i])
  }

}

//--------------------------------------------------
// Map 2D array a to 1D array a_prime
//--------------------------------------------------
void mapping(void)
{
  int x,y;
  int n = gm->n;

  for (x = n-1; x >= 0; x--)
    for (y = n-1; y >= 0; y--)
      gm->a_prime[(n-x) * (n-1-x)/2 + (n-1-y)] = gm->a[x][y];
}

int main(int argc,char **argv) {
  int i,j,p,n;
  double **a,*b, count=1.0;
  unsigned int t1,t2;
  MAIN_INITENV
  if (argc!=3) {
     printf("Usage: pbksb P N\nAborting...\n");
     exit(0);
  }
  gm = (GM*)G_MALLOC(sizeof(GM));
  p = gm->p = atoi(argv[1]);
  gm->n = atoi(argv[2]);
  assert(p > 0);
  assert(p <= 8);
  n = gm->n;
  a = gm->a = (double**)G_MALLOC(n*sizeof(double*));
  for(i = 0; i < n; i++) {
    a[i] = (double*)G_MALLOC(n*sizeof(double));
    for(j = i;j < n;j++){
       a[i][j] = count;
       count++;
    }
  }

  //-----------------------------------------------
  // Create 1D array a_prime and map a to a_prime
  //-----------------------------------------------
  gm->a_prime = (double*)G_MALLOC((n+1)*n/2*sizeof(double))
  mapping();

  b = gm->b = (double*)G_MALLOC(n*sizeof(double));
  for(i = 0; i < n; i++) {
    b[i] = count;
    count++;
  }
  gm->pse = (char*)G_MALLOC(n*sizeof(char));
  for(i = 0; i < n; i++)
    CLEARPAUSE(gm->pse[i])
  for(i = 0; i < p-1; i++)
    CREATE(pbksb)
  CLOCK(t1)
  pbksb();
  WAIT_FOR_END(p-1)
  CLOCK(t2)
  printf("Elapsed: %u us\n",t2-t1);
  for(i = 0; i < n; i++) printf("%lf ", gm->b[i]);
  printf("\n");
  for(i = 0; i < n; i++)
    G_FREE(a[i],n*sizeof(double))
  G_FREE(a,n*sizeof(double*))
  G_FREE(b,n*sizeof(double))
  G_FREE(gm->a_prime, (n+1)*n/2*sizeof(double))
  MAIN_END
  return 0;
}

