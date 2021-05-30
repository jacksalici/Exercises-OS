#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>

typedef int pipe_t[2];
typedef struct{
    char id;      	/* indice figlio (campo v1 del testo) */
	long int occ; 	/* numero caratteri (campo v2 del testo) */
} s_occ;

int r (int f){
    return (int)(f/256);
}


int main(int argc, char *argv[])
{
    
   	int pid;	/* array di pidf */

    int N;   /* numero di caratteri e quindi numero di processi */
    int ff, fn;      /* per open */
    int i, k, j=0;     /* indici, i per i figli! */
    long int cont = 0;     /* per conteggio */
    char c;       /* per leggere dal file */
    pipe_t *pf, *pn;    /* array dinamico di pipe */
    int pidFiglio, pidNipote, status, ritorno;    /* variabili per wait*/
   



    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 3)
    {
        printf("Errore nel numero dei parametri\n");
        exit(1);
        
    }
    
    N = argc-1; // N numero figli
    

    /* allocazione N pipe */
    if ((pf=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL || (pn=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }


    

    /* creo N pipe */
    for (i=0; i < N; i++)
        if (pipe(pf[i]) < 0 || pipe(pn[i]) < 0)
        {
            printf("Errore nella creazione della pipe\n");
            exit(5);
        }
    



    printf("Sono il processo padre con pidf%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < N; i++)
    {/* creazione dei figli */
        
        if ((pid = fork()) < 0)
        {
            printf ("Errore nella fork\n");
            exit(1);
        }
        else if (pid==0)
	    { /* codice figlio */

		    printf("Sono il figlio %d\n", getpid());
		    /* chiusura p inutilizzate */
            for (j=0;j<N;j++)
		    {
                
		    	close (pf[j][0]);
		    	if (j!=i)
		    	    close (pf[j][1]);
		    }

                       
		    if ( (pid = fork()) < 0)
		    {
		    	printf("Errore nella fork di creazione del nipote\n");
		    	exit(-3);
		    }	

		    if (pid == 0) 
		    {
		    /* codice del nipote */
		       

		    	close(pf[i][1]);
				/* Chiusura delle pipe non usate nella comunicazione con il padre  */
				for (j=0; j < N; j++)
				{
					close(pn[j][0]);
					if (i != j) close(pn[j][1]);
				}
		    	
                if ((fn=open(argv[i+1],O_RDWR))<0)
		        {
		    	printf("Impossibile aprire il file %s\n", argv[1]);
		    	/* torniamo -1 che verra' interpreatato come 255 e quindi errore */
		    	exit(-1);
		        }	

            
                while(read(fn, &c, 1) > 0)
                {   
                    if (islower(c)){
                        
                        cont++;
                        lseek(fn, -1L, SEEK_CUR);
                        c = toupper(c);
                        write(fn, &c, 1);
                        lseek(fn, -1L, SEEK_CUR);
                    }
                }

                write(pn[i][1], &cont, sizeof(cont));


		    	exit(r(cont)); 
		    }
            /*codice figlio*/

            /* apertura file */
		    if ((ff=open(argv[i+1],O_RDWR))<0)
		    {
		    	printf("Impossibile aprire il file %s\n", argv[1]);
		    	/* torniamo -1 che verra' interpreatato come 255 e quindi errore */
		    	exit(-1);
		    }	
    

            cont = 0;

            /* conta le occorrenze del carattere assegnato */

            while(read(ff, &c, 1) > 0)
            {
                if (isdigit(c)){
                        cont++;
                        lseek(ff, -1L, SEEK_CUR);
                        c = ' ';
                        write(ff, &c, 1);
                        lseek(ff, -1L, SEEK_CUR);
                    }
            }

            

                       
            /* comunica al figlio successivo usando la i-esima pipe */
            write(pf[i][1],&cont,sizeof(cont));

            pidNipote = wait(&status);
            if (pidNipote < 0)
           {
                printf("Errore wait\n");
                exit(9);
            }
        
            if ((status & 0xFF) != 0)
            printf("Nipote con pidf %d terminato in modo anomalo\n", pidNipote);
            else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  nipote  con  pidf=%d  ha  ritornato %c %d  (se  255 problemi!)\n", pidNipote, ritorno, ritorno);
            }


            exit(r(cont));
        }
    }
    /* padre */
    
    /* chiude tutte le pipe che non usa */
    
    for (k=0; k < N; k++)
    {
        close(pn[k][1]);
        close(pf[k][1]);

    }
   
    for (k=0; k < N; k++){
        
		read(pn[k][0], &cont, sizeof(cont));
		printf("Il nipote di indice %d ha operato %ld sostituzioni di caratteri minuscoli in MAIUSCOLI sullo stesso file %s\n", k, cont, argv[k+1]);
        read(pf[k][0], &cont, sizeof(cont));
		printf("Il figlio di indice %d ha operato %ld sostituzioni di caratteri numerici in carattere spazio sul file %s\n", k, cont, argv[k+1]);
    }

    /* Attesa della terminazione dei figli */
        
    for(i=0;i < N;i++)
    {
        pidFiglio = wait(&status);
        if (pidFiglio < 0)
        {
            printf("Errore wait\n");
            exit(9);
        }
        
        if ((status & 0xFF) != 0)
            printf("Figlio con pidf %d terminato in modo anomalo\n", pidFiglio);
        else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  figlio  con  pidf=%d  ha  ritornato %c %d  (se  255 problemi!)\n", pidFiglio, ritorno, ritorno);
            }
    }
    exit(0);
}

