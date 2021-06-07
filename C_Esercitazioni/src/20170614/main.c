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
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fdr;      						/* per open */
    int i, j;     						/* indici, i per i figli! */
    int cont = 0;     						/* per conteggio */
    char Cx;       						/* per leggere dal file */
    pipe_t *pipedFP;    					/* array dinamico di pipe figlio-padre */
    pipe_t *pipedPF;    					/* array dinamico di pipe padre-figlio */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    long int index = 0; //indice, il primo carattere letto avrà indice 1
	//CONTROLLI 

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;

    /* controllo che l'ultimo parametro sia un carattere*/
    if (strlen(argv[argc-1]) != 1)
        { 
            printf("Errore nella stringa %s che non e' un singolo carattere\n", argv[argc-1]);
            exit(2);
        }
    Cx=argv[argc-1][0];
	//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione 2*N pipe */
    if ((pipedFP=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL || (pipedPF=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	
	//CREAZIONE PIPE 

    /* creo 2*N pipe */
    for (i=0; i < N; i++)
        if (pipe(pipedFP[i]) < 0 || pipe(pipedPF[i]) < 0){
            printf("Errore nella creazione della pipe\n");
            exit(1);
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

			/* chiude tutte le pipe che non usa */
            for (j=0; j < N; j++){
                close(pipedFP[j][0]);
                close(pipedPF[j][1]);
            

                if(j!=i)
                    {
                        close(pipedPF[j][0]);
                        close(pipedFP[j][1]);
                    }
            }
			/* apre il file */
            if ((fdr = open(argv[i+1], O_RDWR)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

            char c; //carattere letto e poi scritto
            
			while(read(fdr, &c, 1) > 0)
            {
                index++;

                //cerco le occorrenze del carattere
                if (c == Cx){
                    //ogni volta che ne trovo una comunico al padre, con anche la posizione
                    cont++;
                    printf("Ho letto %c - %ld\n", Cx, index);
                    if(write(pipedFP[i][1], &index, sizeof(index))==-1){
                        printf("Errore scrittura figlio %d indice %ld\n", i, index); //controllo che la scrittura sia andata a buon fine
                    };
                    read(pipedPF[i][0], &c, sizeof(c));//leggo il carattere che mi manda il padre
                    if(c!='\n'){ //se  è diverso da newline lo sostituisco
                        lseek(fdr, -1L, SEEK_CUR);
                        write(fdr, &c, 1);
                    }
                }
            }



            //esco con il numero di occorrenze del carattere
			exit(cont);
		}
	}

	/* padre */
    
    /* chiude tutte le pipe che non usa */
    for (j = 0; j <N; j++){
        close(pipedFP[j][1]);
        close(pipedPF[j][0]);
    }

	/* legge dalle pipe i messaggi*/
  
    int finito = 0;
    char c, scarto; //carattere

    //finche tutti i file non sono finiti...
    while(!finito){
        finito = 1;
        //...leggo per tutti i file...
        for (i=0; i < N; i++){
            //...la pipeline specifica
            if(read(pipedFP[i][0], &index, sizeof(index))!=0){
            finito=0; //se riesco a leggere il figlio non è ancora terminato

            //leggo un carattere e chiedo all'utente come sostutuirlo
            printf("letto carattere in posizione %ld dal file %s del figlio di indice %d\n", index, argv[i+1], i);
            printf("inserire carattere con cui sostituire, new line se non devo fare nulla\n");
            read(0, &c, 1);
			if (c != '\n') read(0, &scarto, 1); /* se e' stato letto un carattere, bisogna fare una lettura a vuoto per eliminare il carattere corrispondente all'invio */

            //lo scrivo in tutti i casi, cosi il figlio può prosegure
            write(pipedPF[i][1], &c, 1);
            
            }    
        }
    }
	//ATTESA TERMINAZIONE DEI FIGLI   
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
            printf("Il  figlio  con  pid=%d  ha  ritornato  %d carattere trovati (se  255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
