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
    int ff, f;      						/* per open */
    int i, j, q;     						/* indici, i per i figli! */
    int cont = 0;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    char linea[10];
    int lung = 0;
	//CONTROLLI TIPICI
    /*per prima cosa creo un file di nome Camilla*/
    if ((f = open("Camilla", O_WRONLY|O_CREAT|O_TRUNC)) < 0) { 
                printf("Errore nella creazione del file\n");
                exit(1);
            }
    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(2);
    }
    
    Q = argc - 1;


	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione Q pipe */
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
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(-1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* chiude tutte le pipe che non usa (scegli schema chiusura!) */
            for (j=0; j < Q; j++){
                close(piped[j][0]);
                if(j!=q){
                    close(piped[j][1]);
                }
            }

			/* apre il file */
            if ((ff = open(argv[q+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

            int da_mandare = 1;
            //se da_mandare sarà = 0 non manderò la linea
			while (read(ff, &c, 1) > 0){
                //controllo che il primo carattere sia un numero, in caso negativo, la linea non è da_mandare
                if (lung==0){
                    if (!isdigit(c)){
                        da_mandare=0;     
                    }
                }
                if (lung==10){
                    da_mandare=0;
                }

                if (da_mandare){
                    linea[lung]=c;
                    lung++;
                }

                if(c=='\n'){
                    if (da_mandare){
                        for(int i=0; i<lung; i++){
                            write(piped[q][1], &linea[i], 1);
                        }
                        cont++;
                    }

                    lung=0; //resetto i flag e la lunghezza per la linea successiva
                    da_mandare=1;
                }

                
            }


			exit(cont);
		}
	}

	/* padre */
    
    /* chiude tutte le pipe che non usa */
    for (j = 0; j < Q; j++){
        close(piped[j][1]);
    }

	/* leggo dalle pipe i messaggi*/
    for(q=0; q < Q; q++){
        lung=0;
        while(read(piped[q][0], &c, 1)>0){
            //leggo tutti i caratteri della linea e poi la trasformo in stringa
            if(c!='\n'){
               linea[lung]=c;
               lung++;
            }else{
               linea[lung]=0;
               printf("il figlio di indice %d ha mandato la linea %s dal file %s\n", q, linea, argv[q+1]);
               lung=0; //resetto la lunghezza della linea per la riga dopo 
            }
            
        }
    }

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
            printf("Il  figlio  con  pid=%d  ha  mandato  %d linee  (se  >250 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
