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
    	long int c1;		/* valore linee */
	 	int c2;		/* indice figlio */
} s_occ;

int main(int argc, char *argv[]){

   	int *pid;							/* array di pid */

    int M;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
    long int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    s_occ s; //per salvare la struct corrente
    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 2)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    M = argc - 1;


	//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione M pipe */
    if ((piped=(pipe_t *)malloc(M*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(2); 
    }

    /*allocazione array per i pid */
        if ((pid = (int *)malloc(M * sizeof(int))) == NULL)
    {
        printf("Errore allocazione pid\n");
        exit(3);
    }

    /* creo M pipe */
    for (i=0; i < M; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(4);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), M);


    for (i=0; i < M; i++) {
		/* creazione dei figli */
        if ((pid[i] = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(5);
        }

		if (pid[i] == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* chiude tutte le pipe che non usa */

            //schema pipeline: ogni figlio legge dalla pipe i-1 e scrive sulla i
            for (k=0; k < M; k++){
                if (k!= i)
                    close(piped[k][1]);
                if (i==0 || k!=i-1)
                    close(piped[k][0]);
            }

			/* apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio */
            cont = 0; //inizializzo a 0 il n delle linee

            while (read(fd, &c, 1) > 0) //leggo fino alla fine il file associato
            {
                if (c == '\n') //...contando tutte le linee
                    cont++;
            }

            //tutti i figli, eccetto il primo, leggono la struct del figlio "precendente" come numero d'ordine
            if (i!=0){
                if(read(piped[i-1][0], &s, sizeof(s))!=sizeof(s)){
                    printf("Errore in lettura da pipeline");
                    exit(-1);
                }

                //a questo punto si controlla quale file ha il numero maggiore di linee. Nel caso sia il file del figlio corrente, vengono sostituite le info della struct con quelle del figlio corrente
                if (s.c1 < cont){
                      s.c1=cont;
                      s.c2=i;      
                }
            }else{

                //il figlio con i==0 salva le info sulla struct, senza fare controlli perchè è il primo figlio
                s.c1 = cont;
                s.c2 = i;
            }

            //tutti i figli scrivono quidni la struct per farla leggere al figlio successivo, assicurandosi che non ci siano errori in scrittura
            if (write(piped[i][1], &s, sizeof(s))!=sizeof(s)){
                printf("Errore in lettura da pipeline");
                exit(-1);
            }

            //come da specifica si torna il numero d'ordine
            exit(i);
        }
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
    for(k=0; k < M; k++){
        close (piped[k][1]);
        if (k!=M-1){
            close(piped[k][0]);
        }
    }


    //il padre legge il valore mandato dall'ultimo figlio, controllando di ricever etutti i byte che si aspetta e non di meno.
    if (read(piped[M - 1][0], &s, sizeof(s)) != sizeof(s))
    {
        printf("Errore in lettura da pipeline");
        exit(6);
    }

    //stampa a video dei valori arrivati
    printf("Numero massimo di linee: %ld, presenti nel file %s, analizzato dal figlio di ordine %d e pid %d.\n", s.c1, argv[s.c2+1], s.c2, pid[s.c2]);
    
    /* Attesa della terminazione dei figli */
        
    for(i=0;i < M;i++) {
    	pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore wait\n");
	        exit(7);
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
