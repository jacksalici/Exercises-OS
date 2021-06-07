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
    			/*  */
	 			/*  */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
   	
    int N;   							/* numero di caratteri e quindi numero di processi */
    int ff;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
    int cont =0;     						/* per conteggio */
    char cp, cd;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/

	
    /*  numero dei caratteri passati sulla linea di comando */
    if (argc < 3 || (argc-1)%2)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;


	//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione N pipe */
    if ((piped=(pipe_t *)malloc(N/2*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	

	

    /* OBBLIGATORIO: creo N pipe */
    for (i=0; i < N/2; i++) {
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


            //i figli dipari i leggono dalla pipe piped[i/2-1], i figli pari scrivono sulla piped[i/2].      
			for(k=0; k<N/2; k++){
                if(i%2==0){ //se è pari
                    close(piped[k][0]);
                    if(k!=i/2){
                        close(piped[k][1]);
                    }
                }else{
                    close(piped[k][1]);
                    if (k != (i-1)/2)
                    {
                        close(piped[k][0]);
                    }
                }
            }

			/* apre il file F */
            if ((ff = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			if(i%2){
                //processi dispari    
                
                //per prima cosa apro/creo il file, dopo aver creato una strina con il suo nome
                char *str;
                int Fcreato; //pid nuovo file

                if ((str = malloc((strlen(argv[i+1]) + 10) * sizeof(char))) == NULL)
                {
                    printf("Errore nell'allocazione mem");
                    exit(1);
                }
                strcpy(str, argv[i+1]);
                strcat(str, ".MAGGIORE");
                if ((Fcreato = open(str, O_CREAT | O_WRONLY, 0644)) < 0)
                {
                    printf("Errore nella creazione del file %s\n", str);
                    exit(2);
                }


                //leggono un carattere e lo confrontano con quello mandato dal figlio pari collegato
                while(read(ff, &cd, 1)){
                    cont++;
                    read(piped[(i-1)/2][0], &cp, 1);
                    /*write(1, &cd, 1); stampe di debug
                    write(1, &cp, 1);*/
                    if(cd>cp){
                        write(Fcreato, &cd, 1);
                    }else{
                        write(Fcreato, &cp, 1);
                    }
                    
                }

            }else{
                //processi pari

                //leggono un carattere e lo mandano al processo dispari associato
                while (read(ff, &cp, 1)){
                    cont++;
                    write(piped[i/2][1], &cp, 1);
                    
                }
            }


			exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    
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
