
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
typedef int pipe_t[2];
typedef struct{
    char id;      	/* indice figlio (campo v1 del testo) */
	long int occ; 	/* numero caratteri (campo v2 del testo) */
} s_occ;


void bubbleSort(s_occ o[], int dim){
    int i;
    s_occ temp;
    bool ord = false;
    while (dim>1 && !ord){
        ord = true;
        for (i=0; i<dim-1; i++){
            if (o[i].occ > o[i+1].occ){
                temp.occ = o[i].occ;
                temp.id = o[i].id;

                o[i].occ = o[i+1].occ;
                o[i].id = o[i+1].id;

                o[i+1].occ = temp.occ;
                o[i+1].id = temp.id;

                ord=false;    
            }
        }
    }
}




int main(int argc, char *argv[])
{
    
   	int *pid;	/* array di pid */

    int N;   /* numero di caratteri e quindi numero di processi */
    int fdr;      /* per open */
    int i, k, j=0;     /* indici, i per i figli! */
    long int cont;     /* per conteggio */
    char c;       /* per leggere dal file */
    pipe_t *p;    /* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    s_occ *o;   /*array dinamico di strutture*/



    /* numero dei caratteri passati sulla linea di comando */
    if (argc != 2)
    {
        printf("Errore nel numero dei parametri\n");
        exit(1);
        
    }
    
    N = 26; // N numero figli
    

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



    /* creo N pipe */
    for (i=0; i < N; i++)
        if (pipe(p[i]) < 0)
        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
    



    printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < N; i++)
    {/* creazione dei figli */
        
        if ((pid[i] = fork()) < 0)
        {
            printf ("Errore nella fork\n");
            exit(1);
        }
        else if (pid[i]==0)
	    { /* codice figlio */

		printf("Sono il figlio %d\n", getpid());
		/* chiusura p inutilizzate */
        /* array di pipe usate a pipeline da primo figlio, a secondo figlio .... ultimo figlio e poi a padre:
        ogni processo legge dalla pipe i-1 e scrive sulla pipe i */

		for (j=0;j<N;j++)
		{
			if (j!=i)
				close (p[j][1]);
			if ((i == 0) || (j != i-1))
			    close (p[j][0]);
		}


		
		/* apertura file */
		if ((fdr=open(argv[1],O_RDONLY))<0)
		{
			printf("Impossibile aprire il file %s\n", argv[1]);
			/* torniamo -1 che verra' interpreatato come 255 e quindi errore */
			exit(-1);
		}	

        cont = 0;
        
        /* conta le occorrenze del carattere assegnato */
        
        while(read(fdr, &c, 1) > 0)
        {
            if (c == ('a' + i))
                cont++;
        }

        /* allocazione N array */
        if ((o=(s_occ *)malloc(N*sizeof(s_occ))) == NULL)
        {
    	    printf("Errore allocazione pipe\n");
    	    exit(-1); 
        }

        if (i!=0){
            read(p[i-1][0], o, N*sizeof(s_occ)); //manca controllo
        }

        o[i].id = 'a'+ i;
        o[i].occ = cont;

        /* comunica al figlio successivo usando la i-esima pipe */
        write(p[i][1], o, N*sizeof(s_occ));
        exit(c);
        }
    }
    /* padre */
    
    /* chiude tutte le pipe che non usa */
    
    for (k=0; k < N; k++)
    {
        close(p[k][1]);
        if (k!= N-1)
            close (p[k][0]);

    }
   
  
    /* allocazione N array */
        if ((o=(s_occ *)malloc(N*sizeof(s_occ))) == NULL)
        {
    	    printf("Errore allocazione pipe padre\n");
    	    exit(-1); 
        }

    /* legge dalle pipe i messaggi */

    read(p[N-1][0], o, N*sizeof(s_occ));
       
    bubbleSort(o, N);

    for (k=0; k < N; k++){
        printf("il figlio %d ha riportato %ld occorenze del carattere %c\n", pid[k], o[k].occ, o[k].id);
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
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  figlio  con  pid=%d  ha  ritornato %c %d  (se  255 problemi!)\n", pidFiglio, ritorno, ritorno);
            }
    }
    exit(0);
}

