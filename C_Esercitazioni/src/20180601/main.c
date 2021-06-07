#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

typedef int pipe_t[2];



int main(int argc, char *argv[])
{
    int * pid;      						/* pid per fork */

    int N, K;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, k, j;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    int L = 0;
    char linea[255];
	//CONTROLLI TIPICI

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 4 )   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;

     /* controllo numero strettamente positivo*/  
    K = atoi(argv[argc-1]);
    printf("Numero di linee %d\n", K);
    if (K <= 0) 
        {
	        printf("Errore parametro %d\n", K);
	        exit(2); 
        }
    
    int X;
    printf("inserire numero pos minore di %d \n", K);
    scanf("%d", &X);
    if (X<=0 || X>K){
         {
	        printf("Errore parametro %d\n", X);
	        exit(3); 
        }
    }
  
   /* 27.90*/
	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione N pipe */
    if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	 if ((pid=(int *)malloc(N*sizeof(int))) == NULL)    {
    	printf("Errore allocazione pip\n");
    	exit(3); 
    }

    /* : creo N pipe */
    for (i=0; i < N; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    
    for (i=0; i < N; i++) {
		/* creazione dei figli */
        if ((pid[i] = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid[i] == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* chiude tutte le pipe che non usa (scegli schema chiusura!) */
            for (k=0; k < N; k++){
                if(k!=i){
                    close(piped[k][1]);
                }
                close(piped[k][0]);
            }

			/* apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

            int n_riga=1;
            int mandato=0;
            while(read(fd, &c, 1) > 0){
                
                    
                linea[L]=c;
                L++;

               if(c=='\n'){
                   if (n_riga==X){
                        write(piped[i][1], &L, sizeof(L));

                        for (j=0; j < L; j++)
                        {
                            write(piped[i][1], &linea[j], 1);
                        }
                        mandato=1;
                        break;
                   }
                   n_riga++;
                   L=0;
               }
            }
            if(!mandato){
                L=0;
                linea[L]='\n';
                write(piped[i][1], &L, sizeof(L));
                write(piped[i][1], &linea[j], 1);

            }


			exit(L);
		}
	}

	/* padre */
    
    /* chiude tutte le pipe che non usa */
    for (j=0; j < N; j++){
        close(piped[j][1]);
    }

    /*legge dalle pipe del figlio*/
    cont=0;
    for (i=0; i < N; i++) {
        read(piped[i][0], &L, sizeof(L));
        while (1){
            if(read(piped[i][0], &linea[cont], 1)==0)
                break;
            cont++;   
        }
        
        linea[L-1]=0;
        if (L>0){
            printf("il figlio %d ha riportato la linea %s del file %s\n", pid[i], linea, argv[i+1]);
        }else{
            printf("il figlio  %d ha riportato che non esiste quella linea\n", pid[i]);
        }
    }
    /* Attesa della terminazione dei figli */
        
    for(i=0;i < N;i++) {
    	pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore wait\n");
	        exit(9);
    	}
            
        if ((status & 0xFF) != 0)
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  figlio  con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
