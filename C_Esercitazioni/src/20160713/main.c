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
        		
   	int *pid;							/* array di pid */
    int div;
    int *divet;
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j;     						/* indici, i per i figli! */
    int cont=0;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *pipedPF, *pipedFP;    					/* array dinamici di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3 || (argc-1)%2==1)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = (argc - 1)/2;


	//ALLOCAZIONE MEMORIA MALLOC

    /*allocazione N pipe */
    if ((pipedFP=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL || (pipedPF=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	/*allocazione array per i pid */
    if ((pid=(int *)malloc(N*sizeof(int))) == NULL)    {
    	printf("Errore allocazione pid\n");
    	exit(4);
    }


	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo N pipe */
    for (i=0; i < N; i++) {
        if (pipe(pipedFP[i]) < 0 || pipe(pipedPF[i]) < 0)
        {
            printf("Errore nella creazione della pipe\n");
            exit(5);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < N; i++) {
		/*  creazione dei figli */
        if ((pid[i] = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(6);
        }

		if (pid[i] == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* chiude tutte le pipe che non usa*/
            for (j=0; j< N; j++) {

                close(pipedPF[j][1]);
                close(pipedFP[j][0]);

                if(j!=i){
                    close(pipedPF[j][0]);
                    close(pipedFP[j][1]);
                }

            }

			/* apre il file */
            if ((fd = open(argv[2*i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[2*i+1]);
                exit(-1);
            }

			/* leggo il divisore */
            read(pipedPF[i][0], &div, sizeof(div));

            int j=1; //parte da uno per contare correttamente il numero delle lettere.

            while(read(fd, &c, 1) > 0){
                if(j==div){ //ogni volta che troviamo un divisore lo mandiamo al padre e ricominciamo il conto. 
                    cont++;
                    write(pipedFP[i][1], &c, sizeof(c));
                    j=1;
                }else{
                    j++;
                }
            }


            //torniamo il numero di caratteri mandati
			exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
    for(i=0; i < N; i++){
        close(pipedPF[i][0]);
        close(pipedFP[i][1]);
    }

    //allochiamo in memoria il vettore di divisori
    if ((divet = (int *)malloc(N * sizeof(int))) == NULL)
    {
        printf("Errore allocazione vettore di div\n");
        exit(4);
    }

    //chiediamo all'utente un divisore controllando che sia tale
    for(i=0; i < N; i++){
        printf("Inserire un divisore di %s per il file %s.\n", argv[2*i+2], argv[2*i+1]);
        scanf("%d", &divet[i]);
        if(div%atoi(argv[2*i+2])!=0){
            printf("errore divisore\n");
            exit(7);
        }
        write(pipedPF[i][1],&divet[i],4);
    }

    //un carattere per file alla volta leggiamo tutti i caratteri mandati
    int finito=0;
    cont=0;
    while(!finito){
        finito = 1;
        for (i=0; i <N; i++){
            if (read(pipedFP[i][0], &c, 1)>0){
                finito=0;
                printf("Il figlio di indice %d ha letto dal file %s, nella posizione %d, il carattere %c\n", i, argv[2*i+1], divet[i]*cont, c);
            }
        }
        cont++;
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
