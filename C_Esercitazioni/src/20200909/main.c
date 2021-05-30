#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char *argv[])
{
    int pid;      /* pid per fork */
    int Q;   /* numero di caratteri e quindi numero di processi */
    int fdr;      /* per open */
    int q, k, l, i, L;      /* indici, q per q figli! */
    int cont = 0;     /* per conteggio */
    char c;       /* per leggere dal file */
    pipe_t *pipes;    /* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/


//CONTROLLI TIPICI

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 5)
    {
        printf("Errore nel numero dei parametri\n");
        exit(1);
        
    }
    
    Q = argc - 3;
    /* controllo singoli caratteri */
    for (i=0; i < Q; i++)
        if (strlen(argv[i+3]) != 1)
        { 
            printf("Errore nella stringa %s che non e' un singolo carattere\n", argv[i+2]);
            exit(1);
        }

     /* controllo numero strettamente positivo minore di 255*/  
    L = atoi(argv[2]);
    //printf("Numero di linee %d\n", L); //stampa di debug
    if (L <= 0 || L >= 255)  /* il numero deve essere strettamente positivo e strettamente minore di 255 */
        {
	        printf("Errore parametro %d\n", L);
	        exit(2); 
        }


 


//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione Q pipe */
    if ((pipes=(pipe_t *)malloc(Q*sizeof(pipe_t))) == NULL)
    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }



//CREAZIONE PIPE 

    /* creo Q pipe */
    for (q=0; q < Q; q++)
        if (pipe(pipes[q]) < 0)
        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
    

//CREAZIONE FIGLI

    //printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), Q); //stampa di debug
    for (q=0; q < Q; q++)
    {/* creazione dei figli */
        if ((pid = fork()) < 0)
        {
            printf ("Errore nella fork\n");
            exit(1);
            }
        
        
        if (pid == 0) /* figlio */
        {
            printf("Figlio %d con pid %d\n", q, getpid()); //stampa di debug
            
               /*apertura file*/
            if ((fdr = open(argv[1], O_RDONLY)) < 0)
            { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

            /* chiude tutte le pipe che non usa */
            for (k = 0; k < Q; k++)
            {
                close(pipes[k][0]);
                if (k != q)
                    close(pipes[k][1]);
            
            }
            
                       
            /* conta le occorrenze del carattere assegnato */
            cont = 0;
            while(read(fdr, &c, 1) > 0)
            {   
                
                if (c == argv[q+3][0]){
                    cont++;
                }
                else if (c == '\n'){
                    //printf("figlio %d, cont %d\n", q, cont);
                    write(pipes[q][1], &cont, sizeof(cont));
                    
                    cont = 0;
                }
            }

            /* comunica al padre usando la q-esima pipe */
            
            exit(cont);
        }
    }
    /* padre */
    
    /* chiude tutte le pipe che non usa */
    
    for (k=0; k < Q; k++)
    {
        close(pipes[k][1]);
    }

    /* legge dalle pipe q messaggi */
    for(l=0; l<L; l++){
        printf("Linea %d:\n", l+1);
        for (q=0; q < Q; q++)
        {
                read(pipes[q][0], &cont, sizeof(int));
                printf("%d  occorrenze  del  carattere  '%c', %d\n",  cont, argv[q+3][0], q);
	    }
    }	


//ATTESA TERMINAZIONE DEI FIGLI   
        /* Attesa della terminazione dei figli */
        
        for(q=0;q < Q;q++)
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
