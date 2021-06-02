// COSE STANDARD

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
/* mancano delle librerie !*/

typedef int pipe_t[2];
typedef struct{
    int id; 	/* indice figlio (campo c1 del testo) */
	int num; 	/* numero caratteri (campo c2 del testo) */
} s_occ;

int main(int argc, char *argv[])
{
    int pid;      /* pid per fork */
   	int *pid;	/* array di pid */

    int N;   /* numero di caratteri e quindi numero di processi */
    int fdr;      /* per open */
    int i, k;     /* indici, i per i figli! */
    int cont;     /* per conteggio */
    char c;       /* per leggere dal file */
    pipe_t *p;    /* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    

//CONTROLLI TIPICI

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 3)
    {
        printf("Errore nel numero dei parametri\n");
        exit(1);
        
    }
    
    N = argc -2;
    /* controllo singoli caratteri */
    for (i=0; i < N; i++)
        if (strlen(argv[i+2]) != 1)
        { 
            printf("Errore nella stringa %s che non e' un singolo carattere\n", argv[i+2]);
            exit(1);
        }

     /* controllo numero strettamente positivo minore di 255*/  
    H = atoi(argv[argc-1]);
    printf("Numero di linee %d\n", H);
    if (H <= 0 || H >= 255)  /* il numero deve essere strettamente positivo e strettamente minore di 255 */
        {
	        printf("Errore parametro %d\n", H);
	        exit(2); 
        }
    
//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione N pipe */
    if ((p=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

    /* allocazione pid */
    if ((pid=(int *)malloc(N*sizeof(int))) == NULL)
    {
    	printf("Errore allocazione pid\n");
    	exit(4);
    }


    /* padre aggancia le due funzioni (scrivi e salta) che useranno i figli alla ricezione dei segnali inviati dal padre */
    signal(SIGUSR1,scrivi);
    signal(SIGUSR2,salta);

//CREAZIONE PIPE 

    /* creo N pipe */
    for (i=0; i < N; i++)
        if (pipe(p[i]) < 0)
        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
    

//CREAZIONE FIGLI


    printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < N; i++)
    {/* creazione dei figli */
        if ((pid = fork()) < 0)
        {
            printf ("Errore nella fork\n");
            exit(1);
            }
        
        
        if (pid == 0) /* figlio */
        {
            printf("Figlio %d con pid %d\n", i, getpid());
            /* chiude tutte le pipe che non usa */
            for (k = 0; k < N; k++)
            {
                close(p[k][0]);
                if (k != i)
                    close(p[k][1]);
            
            }
            
            /* apre il file */
            if ((fdr = open(argv[1], O_RDONLY)) < 0)
            { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }
            cont = 0;
            
            /* conta le occorrenze del carattere assegnato */
            
            while(read(fdr, &c, 1) > 0)
            {
                if (c == argv[i+2][0])
                    cont++;
            }
            //attendo segnale dal padre
            pause();

            /* comunica al padre usando la i-esima pipe */
            write(p[i][1], &cont, sizeof(cont));
            exit(0);
        }
    }
    /* padre */
    
    /* chiude tutte le pipe che non usa */
    
    for (k=0; k < N; k++)
    {
        close(p[k][1]);
    }
    /* legge dalle pipe i messaggi */
    for (i=0; i < N; i++)
    {
        if (read(p[i][0], &cont, sizeof(int)) > 0)
            printf("%d  occorrenze  del  carattere  %c  nel  file  %s\n",  cont, argv[i+2][0], argv[1]);

//SEGNALI
    /* il padre deve mandare il segnale che corrisponde a scrivi solo al processo di cui gli e' arrivato l'indice, mentre agli altri deve mandare il segnale che corrisponde a salta */

		sleep(1); /* per sicurezza */
		if (i==pip.id)
			kill(pid[i],SIGUSR1);
		else
			kill(pid[i],SIGUSR2);
	}	


//ATTESA TERMINAZIONE DEI FIGLI   
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
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
            else{
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il  figlio  con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", pidFiglio, ritorno);
                }
        }
        exit(0);
    }

/* prima creiamo la pipe di comunicazione fra nipote e figlio */
		  	if (pipe(p) < 0)
                	{	
                        	printf("Errore nella creazione della pipe fra figlio e nipote!\n");
                        	exit(-2);
                	}

			if ( (pid = fork()) < 0)
			{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-3);
			}	
			if (pid == 0) 
			{
			/* codice del nipote */
			printf("Sono il processo nipote del figlio di indice %d e pid %d e sto per recuperare l'ultima linea del file %s\n", j, getpid(), argv[j+1]);
				/* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
				close(piped[j][1]);
				/* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec):  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
				close(0);
				if (open(argv[j+1], O_RDONLY) < 0)
				{
                                printf("Errore nella open del file %s\n", argv[j+1]);
                                exit(-4);
                        	}
				/* ogni nipote deve simulare il piping dei comandi nei confronti del figlio e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
				close(1);
				dup(p[1]); 			
				/* ogni nipote adesso puo' chiudere entrambi i lati della pipe: il lato 0 non viene usato e il lato 1 viene usato tramite lo standard output */
				close(p[0]);
				close(p[1]);
				/* Ridirezione dello standard error su /dev/null (per evitare messaggi di errore a video) */
				close(2);
				open("/dev/null", O_WRONLY);
			
				/* Il nipote diventa il comando wc -1 */				
				execlp("wc", "wc", "-l", (char *)0);
				/* attenzione ai parametri nella esecuzione di wc: aolo -1 e terminatore della lista. */ 
				
				/* Non si dovrebbe mai tornare qui!!: ATTENZIONE avendo chiuso lo standard output e lo standard error NON si possono fare stampe con indicazioni di errori; nel caso, NON chiudere lo standard error e usare perror o comunque write su 2 */
				exit(-1); 
			}