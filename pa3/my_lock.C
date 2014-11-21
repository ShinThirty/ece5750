MAIN_ENV

#define QUEUE

#ifdef TICKET
/* Ticket Lock Implementation */
typedef struct {
  int next_ticket;
  int now_serving;
} ticket_lock;
#endif

#ifdef QUEUE
/* Queue Lock Implementation */
typedef struct {
  char has_lock;
  int *padding;
} qlock_param;

typedef struct {
  qlock_param *slots;
  int next_slot;
} queue_lock;
#endif

typedef struct {
  int *a;
  int p;
  unsigned int *start;
  unsigned int *end;

  // TTS & TS LOCK Declaration
  LOCKDEC(my_lock)
#ifdef TICKET
  // TICKET LOCK Declaration
  ticket_lock *tk_lock;
#endif
#ifdef QUEUE
  // QUEUE LOCK Declaration
  queue_lock *q_lock;
#endif
} GM;

GM *gm;

#ifdef TICKET
void TKLOCK(ticket_lock *lock) {
  LOCK(gm->my_lock)
  int my_ticket = lock->next_ticket++;
  UNLOCK(gm->my_lock)
  while (my_ticket != lock->now_serving);
}

void TKUNLOCK(ticket_lock *lock) {
  lock->now_serving++;
}
#endif

#ifdef QUEUE
void QLOCK(queue_lock *lock, int *my_place) {
  LOCK(gm->my_lock)
  lock->next_slot %= gm->p;
  *my_place = lock->next_slot++;
  UNLOCK(gm->my_lock)
  
  while (lock->slots[*my_place].has_lock == 0);
  lock->slots[*my_place].has_lock = 0;
}

void QUNLOCK(queue_lock *lock, int *my_place) {
  lock->slots[(*my_place+1) % gm->p].has_lock = 1;
}
#endif

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
#ifdef TTS
    LOCK(gm->my_lock)
#endif
#ifdef TS
    TSLOCK(gm->my_lock)
#endif
#ifdef TICKET
    TKLOCK(gm->tk_lock);
#endif
#ifdef QUEUE
    int my_place;
    QLOCK(gm->q_lock, &my_place);
#endif
    /* Critical Section */
    for (j = 0; j < k; j++)
        q++;
#ifdef TTS
    UNLOCK(gm->my_lock)
#endif
#ifdef TS
    TSUNLOCK(gm->my_lock)
#endif
#ifdef TICKET
    TKUNLOCK(gm->tk_lock);
#endif
#ifdef QUEUE
    QUNLOCK(gm->q_lock, &my_place);
#endif
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

  int *a, p, i;
  gm = (GM*)G_MALLOC(sizeof(GM));
  p = gm->p = atoi(argv[1]);

  assert(p > 0);
  assert(p <= 8);
  
  a = gm->a = (int*)G_MALLOC(p*sizeof(int))
  gm->start = (unsigned int*)G_MALLOC(p*sizeof(unsigned int))
  gm->end = (unsigned int*)G_MALLOC(p*sizeof(unsigned int))

  // Initialize the TTS and TS lock
  LOCKINIT(gm->my_lock)
#ifdef TICKET
  // Initialize the ticket lock
  gm->tk_lock = (ticket_lock*)G_MALLOC(sizeof(ticket_lock))
  gm->tk_lock->next_ticket = 0;
  gm->tk_lock->now_serving = 0;
#endif
#ifdef QUEUE
  // Initialize the queue lock
  gm->q_lock = (queue_lock*)G_MALLOC(sizeof(queue_lock))
  gm->q_lock->slots = (qlock_param*)G_MALLOC(p*sizeof(qlock_param))
  for(i = 0; i < p; i++) {
    gm->q_lock->slots[i].padding = (int*)G_MALLOC(256)
    gm->q_lock->slots[i].has_lock = (i == 0) ? 1 : 0;
  }
  gm->q_lock->next_slot = 0;
#endif

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
#ifdef TICKET
  G_FREE(gm->tk_lock, sizeof(ticket_lock))
#endif
#ifdef QUEUE
  for(i = 0; i < p; i++) {
    G_FREE(gm->q_lock->slots[i].padding, 256)
  }
  G_FREE(gm->q_lock->slots, p*sizeof(qlock_param))
  G_FREE(gm->q_lock, sizeof(queue_lock))
#endif
  G_FREE(gm, sizeof(GM))
  MAIN_END
  return 0;
}

