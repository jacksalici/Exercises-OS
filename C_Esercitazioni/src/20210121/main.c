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
    int pid;      						/* pid per fork */
    int Q;   							/* numero di caratteri e quindi numero di processi */
    int ff;      						/* per open */
    int i, j, q;     						/* indici, i per i figli! */
    int k = 0;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/

    char ok = 'y';

	//CONTROLLI TIPICI
    /*numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(2);
    }
    
    Q = argc - 1;


	//ALLOCAZIONE MEMORIA MALLOC

    /*   allocazione Q pipe */
    if ((piped=(pipe_t *)malloc(Q*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }


    /* creo Q pipe */
    for (q=0; q < Q; q++){
        if (pipe(piped[q]) < 0){
            printf("Errore nella creazione della pipe\n");
            exit(4);
        }
    }

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), Q);
    for (q=0; q < Q; q++) {
		/*   creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(-1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* chiude tutte le pipe che non usa (schema ring, ogni figlio legge sulla pipe qesima e scrive su (q+1)%Q) */
            for (j=0; j < Q; j++){
			    if(j != q)
			    	close(piped[j][0]);			// in lettura lascio aperta la pipe con lo stesso indice del figlio
		    	if(j != ((q+1) % Q))
			    	close(piped[j][1]);			// in scrittura lascio aperta la pipe ...
		    }

			/* apre il file */
            if ((ff = open(argv[q+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }
            int z;
            while(read(piped[q][0], &ok, 1)>0){
                z= q+k*Q;
                lseek(ff, z, SEEK_SET);
                if(read(ff, &c, 1)<=0){
                    break;
                }
                printf("Figlio con indice %d e pid %d ha letto il carattere %c\n", q, getpid(), c);
                k++;
                write(piped[(q+1)%Q][1],&ok, 1);

            }
            exit(q);

		}
	}

	/* padre */
    
    printf("Sono il padre (pid: %d)\n", getpid());
	   
    /* chiude tutte le pipe che non usa */
	for(q = 1; q < Q; q++){
		close(piped[q][0]);			// tengo aperto la pipe[0] in scrittura per l'innesco 
		close(piped[q][1]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima scrittura
	}

	write(piped[0][1], &ok, sizeof(char));		// lancio l'innesco
	// sleep(1);
	close(piped[0][1]);							// posso chiudere la pipe iniziale di scrittura


    /* Attesa della terminazione dei figli */
        
    for(i=0;i < Q;i++) {
    	pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore wait\n");
	        exit(9);
    	}
            
        if ((status & 0xFF) != 0)
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  figlio  con  pid=%d  e numero d'ordine  %d è terminato  (se  255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
