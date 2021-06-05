#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#define N 2
int token = 1;
typedef int pipe_t[2];

void scrivi(){
    token = 1;
}

void salta(){
    token = 0;
}

int main(int argc, char *argv[]){
    int pid[2];      						/* pid per fork */
   
   
   
    int F;      						/* per open */
    int i, j;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    pipe_t piped[2];    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per 
    wait*/
    char linea[255];
    int cl; //conteggio linee inviate;
	//CONTROLLI TIPICI

    /* numero dei caratteri passati sulla linea di comando */
    if (argc != 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
  
	/* padre aggancia le due funzioni (scrivi e salta) che useranno i figli alla ricezione dei segnali inviati dal padre */
    signal(SIGUSR1,scrivi);
    signal(SIGUSR2,salta);

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo 2 pipe */
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
            for (j=0; j < N; j++) {
                close(piped[j][0]);
                if(j!=i){
                    close(piped[j][1]);
                }
            }

			/* apre il file */
            if ((F = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }
            
            cont=0;
            cl=0;
			while (read(F, &linea[cont], 1) > 0){
                if(linea[cont]=='\n'){
                    write(piped[i][1], &cont, sizeof(cont));
                    pause();

                    if(token){
                        write(piped[i][1], linea, 255);
                        cl++;
                    }

                    cont= 0;
                }else
                    cont++;    
            }

			exit(cl);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
    for (i=0; i < N; i++){
        close(piped[i][1]);
    }
	/* legge dalle pipe i messaggi e manda segnali*/
    
    int finito = 0;
    int l[2];
    char nlines[2][255];
    cl=0;
    while(!finito){
        finito=1;

        for (i=0; i < N; i++){
            if(read(piped[i][0], &l[i], sizeof(int))>0){
                finito=0;
            }
         }
        
        if(!finito){
            cl++;
            if(l[0]==l[1]){
                for(j=0; j < N; j++){
                    kill(pid[j], SIGUSR1);
                }
                printf("LINEE n%d: UGUALI\n", cl);
                for(j=0; j < N; j++){
                    read(piped[j][0], nlines[j], 255);
                    nlines[j][l[j]]= 0;
                    printf("\tFile %s:\t %s\n", argv[j+1], nlines[j]);
                }
            }else{
                for(j=0; j < N; j++){
                    kill(pid[j], SIGUSR2);
                }
                printf("LINEE n%d: DIVERSE\n", cl);
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
            printf("Il  figlio  con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
