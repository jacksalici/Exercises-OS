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
    	int c1; //pid figlio
        char c2; //secondo carattere della linea
        char c3; //penultimo carattere della linea
} s_occ;

int main(int argc, char *argv[]){
    
   	int pid;							/* array di pid */

    int L;   							/* numero di linee e quindi numero di processi */
    int fd;      						/* per open */
    int q, k;     						/* indici, q per i figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    s_occ s; //per salvare la struttura
    char linea [250];  //per salvare la linea

    /*  numero dei caratteri passati sulla linea di comando */
    if (argc != 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    //controlo che l sia un numero compreso fra 1 e 255
    L = atoi(argv[2]);
    if(L<=0 || L>255){
        printf("Errore nel secondo parametro\n");
        exit(2);
    }


    /*  allocazione L pipe */
    if ((piped=(pipe_t *)malloc(L*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	



    /* creo L pipe */
    for (q=0; q < L; q++) {
        if (pipe(piped[q]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(4);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), L);

    for (q=0; q < L; q++) {
		/* creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(5);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* chiude tutte le pipe che non usa) */
            for (k=0; k < L; k++) {
                close(piped[k][0]);
                if (k!=q){
                    close(piped[k][1]);
                }
            }

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }


            s.c1=getpid(); //associo il pid
            int nlines = 1;
            cont=0;
            while(read(fd, &c, 1) > 0){
                if(nlines==q+1){ //salvo la linea q+esima nel vettore linea
                    linea[cont]=c;
                    cont++;
                }
                if(c=='\n'){
                    nlines++;
                }
            }

            s.c2 = linea[1]; //salvo il secondo e il penultimo carattere nella struct
            s.c3 = linea[cont-2];

            //mandpo la struct al padre ed esco con il numero di linea
            write(piped[q][1], &s, sizeof(s));
			exit(q+1);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
    for (q=0; q < L; q++){
        close(piped[q][1]);
    }

    //il padre legge figlio per figlio la struct e verifica se i caratteri sono uguali. in quel caso stampa a video un messaggio che lo faccia notare all'utente.
    for (q=0; q < L; q++){
        read(piped[q][0], &s, sizeof(s));
        //stampa di debug: printf("caratteri letti dal figlio %d: %c %c\n", q, s.c2, s.c3);
        if(s.c2==s.c3){
            printf("il figlio di indice %d di pid %d ha letto nella linea %d il carattere %c nella prima e penultima posizione\n", q, s.c1, q+1, s.c2);
        }
    }


	//ATTESA TERMINAZIONE DEI FIGLI   
        
    for(q=0;q < L;q++) {
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
