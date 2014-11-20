MAIN_ENV

typedef struct {
  int *a;
  int p;
  unsigned int *start;
  unsigned int *end;

  // TTS LOCK Declaration
  LOCKDEC(my_lock)
} GM;

GM *gm;

void lockEval(void) {
  int pid;
  GET_PID(pid)

  // Experiment variables
  int N, M, k, i, j;
  int p = 0, q = 0;

  N = 100;
  M = 0;
  k = 0;

  CLOCK(gm->start[pid])
  for (i = 0; i < N; i++) {
    LOCK(gm->my_lock)
    // TSLOCK(gm->my_lock)
    /* Critical Section */
    for (j = 0; j < k; j++)
        q++;
    UNLOCK(gm->my_lock)
    // TSUNLOCK(gm->my_lock)
    for (j = 0; j < M; j++)
        p++;
  }
  CLOCK(gm->end[pid])
  gm->a[pid] = p + q;
}

int main(int argc,char **argv) {
  MAIN_INITENV

  if (argc!=2) {
     printf("Usage: my_lock P\nAborting...\n");
     exit(0);
  }

  int *a, p;
  gm = (GM*)G_MALLOC(sizeof(GM));
  p = gm->p = atoi(argv[1]);

  assert(p > 0);
  assert(p <= 8);
  
  a = gm->a = (int*)G_MALLOC(p*sizeof(int))
  gm->start = (unsigned int*)G_MALLOC(p*sizeof(unsigned int))
  gm->end = (unsigned int*)G_MALLOC(p*sizeof(unsigned int))

  // Initiate the lock
  LOCKINIT(gm->my_lock)

  int i;
  for(i = 0; i < p-1; i++)
    CREATE(lockEval)
  lockEval();
  WAIT_FOR_END(p-1)

  // Carry out the max/min computation about execution time
  unsigned int t1 = -1, t2 = 0;
  for (i = 0; i < p; i++) {
    if (gm->start[i] < t1) t1 = gm->start[i];
    if (gm->end[i] > t2) t2 = gm->end[i];
  }

  printf("Elapsed: %u us\n",t2-t1);

  G_FREE(a,p*sizeof(int))
  G_FREE(gm->start,p*sizeof(unsigned int))
  G_FREE(gm->end,p*sizeof(unsigned int))
  G_FREE(gm, sizeof(GM))
  MAIN_END
  return 0;
}

