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

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
   	
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
    int cont=0;     						/* per conteggio */
    
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    char linea[250];                //per salvare la linea;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
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

	
    /*  creo N pipe */
    for (i=0; i < N; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);


    for (i=0; i < N; i++) {

		/*  creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* chiude tutte le pipe che non usa*/
            for (k=0; k < N; k++){
                close(piped[k][0]);
                if (k!=i){
                    close(piped[k][1]);
                }
            }

			/* apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }


            int tmp = 0;
			/* eseguo codice figlio */
            while(read(fd, &linea[tmp], 1) > 0){
                cont = tmp;
                if (linea[tmp]=='\n'){
                    tmp=0;
                }else{
                    tmp++;
                    cont++;
                }
            }
            cont++;
            printf("%d\n", cont);
            write(piped[i][1], linea, 249);


			exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());

    //crea il file

    
    if ((fd = open("Fcreato", O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0)
    {
        printf("Errore nella creazione del file \n");
        exit(2);
    }

    /* chiude tutte le pipe che non usa */
    for (k=0; k < N; k++){
        close(piped[k][1]);
    }

    
	for (i=0; i < N; i++) {
        read(piped[i][0], linea, 249);
        cont = 0;
        do{
            write(fd, &linea[cont], 1);
            cont++;
            if (linea[cont - 1] == '\n'){
                break;
            }
        }while(1);


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
