/* Soluzione della parte C del compito del 13 Giugno 2018 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef int pipe_t[2];
typedef struct{
        int pid; 	/* pid figlio (campo c1 del testo) */
	int lunghezza; 	/* lunghezza linea (campo c2 del testo) */
		} s;

typedef struct{
        int pid; 	/* pid figlio */
	int indice;	/* indice del figlio: CAMPO AGGIUNTO DAL PADRE */
	int lunghezza; 	/* lunghezza linea */
		} s_padre;

void bubbleSort(s_padre v[],int dim) /* il tipo degli elementi NON e' un semplice int come riportato sul testo, ma deve essere il tipo s_padre (appositamente definito) */
{
int i;
int ordinato=0;
s_padre a;  /* variabile di appoggio per fare lo scambio */
        while (dim>1 && ordinato!=1)
        {
                ordinato=1;
                for (i=0;i<dim-1;i++)
                if (v[i].lunghezza > v[i+1].lunghezza) /* chiaramente il confronto va fatto sul campo lunghezza della struttura */
                {  /* viene effettuato in questo caso un ordinamento in senso crescente */
                        /* scambio gli elementi */
                        a=v[i];
                        v[i]=v[i+1];
                        v[i+1]=a;
                        ordinato=0;
                }
                dim--;
        }
}/* fine bubblesort */

int main (int argc, char **argv)
{
int N; 			/* numero di file */
int Y; 			/* numero di linee */
int pid;		/* pid per fork */
pipe_t *pipes;		/* array di pipe usate a pipeline da primo figlio, a secondo figlio .... ultimo figlio e poi a padre: ogni processo (a parte il primo) legge dalla pipe i-1 e scrive sulla pipe i */
int i,j; 		/* contatori */
int fd; 		/* file descriptor */
int pidFiglio, status, ritorno;	/* per valore di ritorno figli */
char ch;  		/* carattere letto da linea */
s *cur; 		/* array di strutture usate dai figli */
s_padre *cur_padre; 	/* array di strutture usate dal padre */
int nr;			/* variabili per salvare valori di ritorno di read su pipe */

/* controllo sul numero di parametri almeno 2 file e un carattere */
if (argc < 4)
{
	printf("Errore numero di parametri\n");
	exit(1);
}

/* calcoliamo valore di Y e lo controlliamo */
Y=atoi(argv[argc-1]);
if (Y < 0)
{
   printf("Errore valore di Y\n");
   exit(2);
}

/* stampa di debugging */
printf("Valore di Y %d\n", Y);

N = argc-2;
printf("Numero di processi da creare %d\n", N);

/* allocazione pipe */
if ((pipes=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
{
	printf("Errore allocazione pipe\n");
	exit(3); 
}

/* creazione pipe */
for (i=0;i<N;i++)
	if(pipe(pipes[i])<0)
	{
		printf("Errore creazione pipe\n");
		exit(4);
	}

/* creazione figli */
for (i=0;i<N;i++)
{
	if ((pid=fork())<0)
	{
		printf("Errore creazione figli\n");
		exit(5);
	}
	else if (pid==0)
	{ 	/* codice figlio */
		printf("Sono il figlio %d e sono associato al file %s\n", getpid(), argv[i+1]);
		/* nel caso di errore in un figlio decidiamo di ritornare un valore via via crescente rispetto al massimo valore di i (cioe' N-1) */
		/* chiusura pipes inutilizzate */
		for (j=0;j<N;j++)
		{
			if (j!=i)
				close (pipes[j][1]);
			if ((i == 0) || (j != i-1))
				close (pipes[j][0]);
		}	
 
		/* allocazione dell'array di strutture specifico di questo figlio */
		/* creiamo un array di dimensione i+1 anche se leggeremo i strutture, dato che poi ci servira' riempire la i+1-esima struttura! */
		if ((cur=(s *)malloc((i+1)*sizeof(s))) == NULL)
		{
        		printf("Errore allocazione cur\n");
        		exit(N);
		}
		/* inizializziamo l'ultima struttura che e' quella specifica del figlio corrente (nel caso del primo figlio sara' l'unica struttura */
		cur[i].pid = getpid();
		cur[i].lunghezza= 0;
	
		/* apertura file */
		if ((fd=open(argv[i+1],O_RDONLY))<0)
		{
			printf("Impossibile aprire il file %s\n", argv[i+1]);
			exit(N+1);
		}
		while(read(fd,&ch,1)>0)
		{
			/* se leggo un carattere, incremento la lunghezza della linea */
			cur[i].lunghezza++;
			if (ch == '\n')	
		 	{
				/* se siamo a fine linea si deve leggere dal figlio precedente l'array (se non siamo il primo figlio) e mandare la struttura al figlio successivo */ 
				if (i!=0)
				/* lettura da pipe dell'array di strutture per tutti i figli a parte il primo */
 				{		
					nr=read(pipes[i-1][0],cur,i*sizeof(s));
					if (nr != i*sizeof(s))
        				{	
        					printf("Figlio %d ha letto un numero di strutture sbagliate %d\n", i, nr);
        					exit(N+2);
        				}	
 					/*
					for(j=0;j<i;j++)
						printf("HO ricevuto da figlio di pid %d la lunghezza %d\n", cur[j].pid, cur[j].lunghezza);
  					*/
				}		 

				/* tutti i figli mandano in avanti, l'ultimo figlio manda al padre un array di strutture (i ricevute dal processo precedente e la i+1-esima la propria */
				write(pipes[i][1],cur,(i+1)*sizeof(s));
				cur[i].lunghezza= 0;	/* si deve azzerare nuovamente la lunghezza per la prossima linea */
			}
		}	
		exit(i); /* ogni figlio deve ritornare al padre il proprio indice */
	}
} /* fine for */

/* codice del padre */
/* chiusura pipe: tutte meno l'ultima in lettura */
for (i=0;i<N;i++)
{
	close (pipes[i][1]);
	if (i != N-1) close (pipes[i][0]);
}

/* allocazione dell'array di strutture specifico per il padre per la read */
/* creiamo un array di dimensione N quanto il numero di figli! */
if ((cur=(s *)malloc(N*sizeof(s))) == NULL)
{
       	printf("Errore allocazione cur nel padre\n");
       	exit(6);
}

/* allocazione dell'array di strutture specifico per il padre per l'ordinamento */
/* creiamo un array di dimensione N quanto il numero di figli! */
if ((cur_padre=(s_padre *)malloc(N*sizeof(s_padre))) == NULL)
{
       	printf("Errore allocazione curi_padre nel padre\n");
       	exit(7);
}

/* il padre deve leggere via via gli array di strutture che gli arrivano dall'ultimo figlio: uno per ogni linea */
for (j=1;j<=Y;j++)
{
	nr=read(pipes[N-1][0],cur,N*sizeof(s));
	if (nr != N*sizeof(s))
	{
        	printf("Padre ha letto un numero di strutture sbagliate %d\n", nr);
        	exit(8);
	}
	/* il padre deve copiare l'array ricevuto nel proprio array */
	for (i=0;i<N;i++)
	{
		cur_padre[i].pid = cur[i].pid;
		cur_padre[i].indice = i;
		cur_padre[i].lunghezza = cur[i].lunghezza;
	}
	/* ordiniamo l'array cur_padre */
	bubbleSort(cur_padre,N);
	/* stampiamo le informazioni */
	for (i=0;i<N;i++)
		printf("Il figlio con pid %d per il file %s ha calcolato per la linea %d la lunghezza %d\n", cur_padre[i].pid, argv[cur_padre[i].indice + 1], j, cur_padre[i].lunghezza);
}

/* Il padre aspetta i figli */
for (i=0; i < N; i++)
{
        pidFiglio = wait(&status);
        if (pidFiglio < 0)
        {
                printf("Errore in wait\n");
                exit(9);
        }

        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        { 
		ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d\n", pidFiglio, ritorno);
        }
}
exit(0);
}
