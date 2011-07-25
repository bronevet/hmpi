#include "hmpi.hpp"

void tmain(){

  int p,r;
  HMPI_Comm_rank (HMPI_COMM_WORLD, &r);
  HMPI_Comm_size (HMPI_COMM_WORLD, &p);
  
  printf("rank %i of %i\n", r, p);


//#define PINGPONG
#ifdef PINGPONG
#define MAXSIZE 1024 //*1024*5
#define SAMPLES 1000
  char *sbuf = (char*)malloc(MAXSIZE);
  char *rbuf = (char*)malloc(MAXSIZE);
  if(p>=2) {
    if(r == 0) {
      for(int sample=0; sample<SAMPLES; sample++) {
        printf("yay 0\n"); fflush(stdout);
        HMPI_Send(sbuf, 1, MPI_BYTE, 1, 99, HMPI_COMM_WORLD);
        HMPI_Recv(rbuf, 1, MPI_BYTE, 1, 99, HMPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    } else {
      for(int sample=0; sample<SAMPLES; sample++) {
        printf("yay 1\n"); fflush(stdout);
        //printf("begin size: %i, %x, sample: %i, %x\n", size, &size, sample, &sample);
        HMPI_Recv(sbuf, 1, MPI_BYTE, 0, 99, HMPI_COMM_WORLD, MPI_STATUS_IGNORE);
        HMPI_Send(rbuf, 1, MPI_BYTE, 0, 99, HMPI_COMM_WORLD);
        //printf("end size: %i, sample: %i\n", size, sample);
      }
    }

    // end warmup
#if 0
    for(int size=1; size<MAXSIZE; size*=2) {
      if(r == 0) {
        double t=-MPI_Wtime();
        for(int sample=0; sample<SAMPLES; sample++) {
          HMPI_Send(sbuf, size, MPI_BYTE, 1, 99, HMPI_COMM_WORLD);
          HMPI_Recv(rbuf, size, MPI_BYTE, 1, 99, HMPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        t+=MPI_Wtime();
        printf("%i %f us %f MiB/s\n", size, t/SAMPLES*1e6, (double)size/(1024*1024)/(t/SAMPLES));
      } else {
        for(int sample=0; sample<SAMPLES; sample++) {
          //printf("begin size: %i, %x, sample: %i, %x\n", size, &size, sample, &sample);
          HMPI_Recv(sbuf, size, MPI_BYTE, 0, 99, HMPI_COMM_WORLD, MPI_STATUS_IGNORE);
          HMPI_Send(rbuf, size, MPI_BYTE, 0, 99, HMPI_COMM_WORLD);
          //printf("end size: %i, sample: %i\n", size, sample);
        }
      }
    }
#endif
  }
#endif
//#define TEST
#ifdef TEST
  int buf=0;
  if(p>=2) 
  if(r == 0) {
    buf = 100;
    HMPI_Send(&buf, 1, MPI_INT, 1, 99, HMPI_COMM_WORLD);
    printf("send finished\n");
  } else if(r == 1) {
    HMPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, 99, HMPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("buf: %i [%x] (should be 100 ;-)\n", buf, &buf);
  }

  int x=1, y=0;
  HMPI_Allreduce(&x, &y, 1, MPI_INT, MPI_SUM, HMPI_COMM_WORLD);
  printf("[%i] buf: %i\n", r, y);

  if(r == 0) y=1;
  HMPI_Bcast(&y, 1, MPI_INT, 0, HMPI_COMM_WORLD);
  printf("[%i] bcast buf: %i\n", r, y);
#endif
#define TEST2
#ifdef TEST2
    int* sendbuf = (int*)malloc(sizeof(int) * 8192);
    int* recvbuf = (int*)malloc(sizeof(int) * 8192);
#if 0
    for(int i = 0; i < p; i++) {
        sendbuf[i] = i;
    }

    HMPI_Scatter(sendbuf, 1, MPI_INT, recvbuf, 1, MPI_INT, 0, HMPI_COMM_WORLD);

    printf("[%i] scatter: %d %s\n", r, *recvbuf, r == *recvbuf ? "GOOD" : "BAD");

    fflush(stdout);
    HMPI_Barrier(HMPI_COMM_WORLD);

    sendbuf[0] = r + 10;
    
    HMPI_Gather(sendbuf, 1, MPI_INT, recvbuf, 1, MPI_INT, 0, HMPI_COMM_WORLD);

    if(r == 0) {
      for(int i = 0; i < p; i++) {
        printf("[%i] gather %d: %d %s\n", r, i, recvbuf[i], (i + 10) == recvbuf[i] ? "GOOD" : "BAD");
      }
    }
#endif
    printf("%d sendbuf %p recvbuf %p\n", r, sendbuf, recvbuf);
    fflush(stdout);
    HMPI_Barrier(HMPI_COMM_WORLD);

    for(int i = 0; i < p; i++) {
        sendbuf[i] = r;
        recvbuf[i] = -1;
    }

    HMPI_Alltoall(sendbuf, 1, MPI_INT, recvbuf, 1, MPI_INT, HMPI_COMM_WORLD);

    for(int i = 0; i < p; i++) {
        printf("[%i] alltoall %d: %d %s\n", r, i, recvbuf[i], i == recvbuf[i] ? "GOOD" : "BAD");
    }
    fflush(stdout);
#endif
  HMPI_Finalize();
}


int main(int argc, char** argv) {

  HMPI_Init(&argc, &argv, 4, &tmain);


}