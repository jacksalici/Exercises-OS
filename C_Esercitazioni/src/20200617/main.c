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

    int B;   							/* numero di blocchi e quindi numero di processi */
    int L; //dimensione 
    int F, Fc;      						/* per open */
    int q, k;     						/* indici, q per q figli! */
    
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    char *buff;

    //per prima cosa apro/creo il file, dopo aver creato una strina con il suo nome
    char * str;
    if((str= malloc((strlen(argv[1])+8)*sizeof(char)))==NULL){
        printf("Errore nell'allocazione mem");
        exit(1);
    }
    strcpy(str, argv[1]);
    strcat(str, ".Chiara");
    if ((Fc = open(str, O_CREAT|O_WRONLY, 0644)) < 0)
    {
        printf("Errore nella creazione del file %s\n", str);
        exit(2);
    }

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(3);
    }
    
    B=atoi(argv[2]);
    L=atoi(argv[3]);
    if (B<=0||L<=0){
        printf("Errore nei parametri\n");
        exit(4);
    }


	//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione B pipe */
    if ((piped=(pipe_t *)malloc(B*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(5); 
    }


    if ((buff =malloc(B * 1)) == NULL)
    {
        printf("Errore allocazione buffer\n");
        exit(6);
    }

    /* creo B pipe */
    for (q=0; q < B; q++) {
        if (pipe(piped[q]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(7);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), B);
    for (q=0; q < B; q++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(8);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* chiude tutte le pipe che non usa*/
            for (k=0; k<B; k++){
                close(piped[k][0]);
                if(k!=q){
                    close (piped[k][1]);
                }
            }

			/* apre il file */
            if ((F = open(argv[1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			
            //mi posiziono alla posizione giusta e leggo il buffer
            lseek(F, q*L/B-1, SEEK_SET);
            read(F, buff, L/B);
        
            write(piped[q][1], &buff[L/B-1], 1);


			exit(L/B);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
    for (q=0; q<B; q++){
        close(piped[q][1]);
    }


	/* legge dalle pipe i messaggi*/

    for (q=0; q<B; q++){
        while (read(piped[q][0], &c, 1)){
            write(Fc, &c, 1);
  
        }
    }



    /* Attesa della terminazione dei figli */
        
    for(q=0;q < B;q++) {
    	pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore wait\n");
	        exit(9);
    	}
            
        if ((status & 0xFF) != 0)
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  figlio  %d con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", q, pidFiglio, ritorno);
        }
    }
    exit(0);
}
