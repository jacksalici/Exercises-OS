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

typedef struct{
    	int c1;		/* primo conteggio */
	 	int c2;		/* secondo conteggio */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
 
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
   
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    s_occ s; /* per salvare la struct corrente */
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3 || (argc-1)%2==1)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;


	//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione N pipe */
    if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	


    /* creo N pipe */
    for (i=0; i < N; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
   
   
    for (i=0; i < N; i++) {
		/* creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

            //chiusura pipe
           
            for (k = 0; k < N; k++)
            {
                close(piped[k][0]);
                if (k != i)
                    close(piped[k][1]);
            }

            /* apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio */

            int pos = 0; //inizializzo la struttura e la posizione dei caratteri nel file
            s.c1=0;
            s.c2=0;
                       
            while (read(fd, &c, 1) > 0){
                //il comportotamento cambia fra i figli pari e quelli dispari    
                if(i%2){
                    //i figli dispari controllano se i caratteri nelle posizioni dispari sono pari o dispari e aumenta rispettivamente il campo 2 o il campo 1

                    if (pos % 2 == 1)
                    {
                        //caratteri di posizione dispari
                        if (c % 2 == 0)
                        {
                            s.c2++;
                        }
                        else
                        {
                            s.c1++;
                        }
                    }
                }else{
                    //figli pari controllano se i caratteri nelle posizioni pari sono pari o dispari e aumenta rispettivamente il campo 1 o il campo 2

                    if (pos % 2 == 0)
                    {
                        //caratteri di posizione pari
                        if (c % 2 == 0)
                        {
                            s.c1++;
                        }else{
                           s.c2 ++;
                       }
                   }
                }

                pos++;

            }

            write(piped[i][1], &s, sizeof(s)); //mando la struct al padre

            
            //esco con 0 o 1 a seconda di quale conteggio sia maggiore, seguendo le specifiche
            if (s.c1>s.c2)
			    exit(0);
            else
                exit(1);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */

    for (k = 0; k < N; k++)
    {
        close(piped[k][1]);
    }

    /* legge dalle pipe i messaggi, prima tutti i figli pari poi tutti i figli dipari*/
    for (k = 0; k < 2; k++){
        for(i=0; i < N; i++){
            if ((i+k)%2 == 0){ //al primo ciclo esterno questo verificherà solo i pari, al secondo ciclo solo quelli dispari
                read(piped[i][0], &s, sizeof(s));
                //leggo dalla pipe e stampo
                printf("FIGLIO D'ORDINE %d, campo 1 = %d, campo 2 = %d\n", i, s.c1, s.c2);
            }
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
