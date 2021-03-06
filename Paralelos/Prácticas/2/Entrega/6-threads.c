#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#define N_THREADS 4
//Dimension por defecto de las matrices
int N=100,jobs_thread;
double average;
double *A,*B,*C;

struct thread_data{
    int  tid;
    int  result_a;
    int  result_b;
};

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void *t_average(void *arg){
    struct thread_data *t_data;
    t_data = (struct thread_data*) arg; 
    int i,j,a=0,b=0,eval=(t_data->tid+1)*jobs_thread;
    for(i=t_data->tid*jobs_thread;i<eval;i++){
	for(j=0;j<N;j++){
	    a += A[i,j];
	    b += B[i,j];
	}
    }
    t_data->result_a=a;
    t_data->result_b=b;

}


void *prod_matrix(void *arg){
    struct thread_data *t_data;
    t_data = (struct thread_data*) arg; 
    int i,j,k,a=0,b=0,eval=(t_data->tid+1)*jobs_thread,tmp;
    for(i=t_data->tid*jobs_thread;i<eval;i++){
     for(j=0;j<N;j++){
	 tmp=0;
 	for(k=0;k<N;k++){
            tmp+=A[i,k]*B[k,j]*average;
	}
	C[i,j]=tmp;
     }
  }
}


int main(int argc,char*argv[]){
 int i,j,k;
 int check=1;
 double timetick;

 //Controla los argumentos al programa
 if ((argc != 2) || ((N = atoi(argv[1])) <= 0) )
  {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
       A[i,j] = 1;
       B[i,j] = 1;
   }
  }   

  // Defino los thread
  pthread_t thread[N_THREADS];
  struct thread_data data[N_THREADS];
  
  pthread_attr_t attr;
  int rc;
  jobs_thread=N/N_THREADS;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  timetick = dwalltime();

  for(i=0; i<N_THREADS; i++)
      {
	  data[i].tid=i;
	  rc = pthread_create(&thread[i], &attr, t_average, (void *) &data[i]);
	  if (rc){
	      printf("ERROR; return code from pthread_create() is %d\n", rc);
	      exit(-1);
	  }
}
  double a,b = 0;
  for(i=0; i<N_THREADS; i++)
      {
	  pthread_join(thread[i], NULL);
	  a += data[i].result_a;
	  b += data[i].result_b;
      }

  average = (a/(N*N))*(b/(N*N));
  
  for(i=0; i<N_THREADS; i++)
      {
	  rc = pthread_create(&thread[i], &attr, prod_matrix, (void *)&data[i]);
	  if (rc){
	      printf("ERROR; return code from pthread_create() is %d\n", rc);
	      exit(-1);
	  }
      }
  
  for(i=0; i<N_THREADS; i++)
      {
	  pthread_join(thread[i], NULL);
      }
    

  //printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	//check=check&&(getValor(C,i,j,ORDENXFILAS)==N);
       check=check&&(C[i,j]==N);
   }
  }   

  if(check){
      printf("%f\n",dwalltime() - timetick);
	//   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }

 free(A);
 free(B);
 free(C);
 pthread_exit(NULL);
 return(0);
}
