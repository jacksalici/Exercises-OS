/* soluzione parte C esame del 19 Giugno 2019: VERSIONE CHE CALCOLA IL MASSIMO */
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

typedef int pipe_t[2]; 		/* tipo di dato per contenere i file descriptors di una pipe */

/* VARIABILI GLOBALI */
int *finito; 	/* array dinamico per indicare i figli che sono terminati */
		/* la semantica di questo array e' che ogni elemento vale 0 se il corrispondente processo NON e' finito, altrimenti vale 1 */
int N; 		/* numero di processi figli: deve essere una variabile globale perche' deve essere usata dalla funzione finitof */

int finitof()
{
/* questa funzione verifica i valori memorizzati nell'array finito: appena trova un elemento uguale a 0 vuole dire che non tutti i processi figli sono finiti e quindi torna 0; tornera' 1 se e solo se tutti gli elementi dell'array sono a 1 e quindi tutti i processi sono finiti */
	int i;
	for (i=0; i < N; i++)
		if (!finito[i])
			/* appena ne trova uno che non ha finito */
			return 0; /* ritorna falso */
	return 1;
}

int main(int argc, char **argv) 
{
	int pid;		/* variabile per fork */
	pipe_t *pipe_fp;	/* array di pipe per la comunicazione figli-padre*/
	pipe_t *pipe_pf;	/* array di pipe per l'indicazione padre-figli */
	int fd;			/* variabile per open */
	char linea[250];	/* variabile per leggere le linee dai file (bastano 250 carattere, vedi testo) */
	char ch;		/* variabile usata dal padre per leggere i primi caratteri delle linee inviati dai figli */
	char chMax;		/* variabile per tenere traccia del carattere di codice ASCII massimo */
	char chControllo;	/* variabile per inviare indicazione ai figli */
	int indice; 		/* usata dal padre per tenere traccia dell'indice del figlio che ha calcolato il massimo */
	int stampe;		/* numero di stampe fatte dai figli che va ritornato al padre */
	int status, pidFiglio, ritorno;	/* per wait */
	int i, j;		/* indici per cicli */
	int nr, nw;		/* per controlli read e write su/da pipe */

/* Controllo sul numero di parametri */
if (argc < 3) 
{
	printf("Errore numero parametri %d\n", argc);
	exit(1);
}

N = argc - 1; 

/* allocazione memoria dinamica per finito */
finito = (int *) malloc(sizeof(int) * N);
if(finito == NULL)
{
	printf("Errore nella allocazione della memoria per array finito\n");
	exit(2);
}
	
/* ... e inizializzazione a 0: all'inizio nessun figlio e' finito */
memset(finito, 0, N*sizeof(int));

/* allocazione memoria dinamica per pipe_fp e pipe_pf */
pipe_fp=malloc(N*sizeof(pipe_t));
pipe_pf=malloc(N*sizeof(pipe_t));
if ((pipe_fp == NULL) || (pipe_pf == NULL))
{
	printf("Errore nelle malloc\n");
	exit(3);
}

/* creazione delle pipe */
for (i=0; i < N; i++) 
{
	if (pipe(pipe_fp[i])!=0) 
	{
		printf("Errore creazione delle pipe figli-padre\n");
		exit(4);
	}
	if (pipe(pipe_pf[i])!=0) 
	{
		printf("Errore creazione delle pipe padre-figli\n");
		exit(5);
	}
}

/* creazione dei processi figli */
for (i=0; i < N; i++) 
{
	pid=fork();
 	if (pid < 0)  /* errore */
   	{
		printf("Errore nella fork con indice %d\n", i);
      		exit(6);
   	}
	if (pid == 0) 
	{
 		/* codice del figlio */
		/* stampa di debugging */
      		printf("Figlio di indice %d e pid %d associato al file %s\n",i,getpid(), argv[i+1]);
      		/* chiudiamo le pipe che non servono */
      		/* ogni figlio scrive solo su pipe_fp[i] e legge solo da pipe_pf[i] */
      		for (j=0;j<N;j++)
      		{
        		close(pipe_fp[j][0]);
        		close(pipe_pf[j][1]);
        		if (j!=i)
        		{
        			close(pipe_fp[j][1]);
        			close(pipe_pf[j][0]);
        		}
      		}

		fd=open(argv[i+1], O_RDONLY);
		if (fd < 0) 
		{
			printf("Impossibile aprire il file %s\n", argv[i+1]);
			exit(-1); /* in caso di errore torniamo -1 che verra' interpretato dal padre come 255 che non e' un valore accettabile */
		}

		/* inizializziamo stampe */
		stampe=0;
		j=0; /* inizializzazione indice per la linea */
	 	while (read(fd, &linea[j], 1)) 
		{
			if (linea[j] == '\n') /* se siamo a fine linea */
			{
				/* inviamo il primo carattere al padre */
				nw=write(pipe_fp[i][1], &linea[0], 1);
				if (nw != 1)
    				{		
                        		printf("Impossibile scrivere sulla pipe per il processo di indice %d\n", i);
                        		exit(-1); 
               	 		}
				/* aspettiamo dal padre se dobbiamo scrivere o meno */
				nr=read(pipe_pf[i][0], &chControllo, 1);
				if (nr != 1)
    				{		
                        		printf("Impossibile leggere dalla pipe per il processo di indice %d\n", i);
                        		exit(-1); 
         		 	}		
				/* printf("FIGLIO indice %d ho ricevuto dal padre %c\n", i, chControllo); */
				if (chControllo =='S') 
				{
					/* autorizzato, scriviamo */
					/* ma prima convertiamo la linea in stringa: ATTENZIONE IL TERMINATORE DI LINEA DEVE RIMANERE E QUINDI USIAMO L'INDICE j+1! */
					linea[j+1]='\0';
					printf("Sono il figlio di indice %d e pid %d e ho trovato una linea con un primo carattere %c con codice ASCII maggiore degli altri nel file %s. Ecco la linea:\n%s", i, getpid(), linea[0], argv[i+1], linea);
					stampe++;	/* incrementiamo il numero di stampe fatte */
				} 
				else 
				{  /* nulla, si continua a leggere */
					;
				}
				j = 0; /* riportiamo a 0 l'indice della linea */
			}
			else
				j++; 	/* incrementiamo il valore di j per il prossimo carattere */
		}	
		exit(stampe); /* torniamo il numero di stampe fatte (supposto dal testo < di 255) */
	}
}

/*codice del padre*/
/* chiudiamo le pipe che non usiamo */
for (i=0;i<N;i++)
{
   close(pipe_fp[i][1]);
   close(pipe_pf[i][0]);
}

/* continuiamo a ciclare su tutti i figli in ordine, finche' tutti non hanno finito */
/* printf("PADRE valore della funzione finitof=%d\n", finitof()); */
while(!finitof())
{
	chMax=-1; /* ATTENZIONE: il valore del massimo, va resettato per ogni insieme di valori letti dal padre */
	for (i=0; i < N; i++) 
	{
		/* tentiamo di leggere il carattere dal figlio i-esimo: contestualmente viene aggiornato il valore del corrispondente elemento dell'array finito */
		finito[i] = (read(pipe_fp[i][0], &ch, 1) != 1);  /* nel caso la read torni un valore diverso da 1, il corrispondente valore di finito viene settato a 1 dato che la condizione e' vera! */
		/* printf("PADRE per indice %d ha calcolato finito = %d\n", i, finito[i]); */

		if (!finito[i]) 
		{
			if (ch > chMax)
			{
				/* dobbiamo aggiornare il massimo */
				chMax = ch;
				/* debbiamo tenere conto dell'indice del processo che ha inviato il massimo */
				indice = i;
			}
		}
		/* else  printf("PADRE il figlio di indice %d non ha inviato nulla\n", i); */
	}
	/* bisogna mandare ai figli l'indicazione giusta (S per scrivere, N per NON scrivere) */
	/* OSSERVAZIONE IMPORTANTE: IN QUESTA SOLUZIONE NON BASTA LA VARIABILE FINITO DATO CHE LA FASE DI SCRITTURA DEL PADRE AI FIGLI VIENE FATTA NEL CICLO FOR CHE SEGUE E QUINDI IL PADRE DEVE AVERE MODO DI FARE LA SCRITTURA SE E SOLO SE IL FIGLIO GLI HA INVIATO QUALCOSA E QUINDI NON E' FINITO, ALTRIMENTI (IN CASO DI ASSENZA DI QUESTO CONTROLLO) SE UN FIGLIO E' FINITO E IL PADRE GLI MANDA COMUNQUE IL CARATTERE DI CONTROLLO, IL PADRE RICEVEREBBE UN SEGNALE SIGPIPE E MORIREBBE (A MENO CHIARAMENTE DI NON GESTIRE QUESTO SEGNALE CON UNA FUNZIONE HANDLER SPECIFICA)! */ 
	for (i=0; i < N; i++) 
	{
		if (i == indice) /* figlio che deve scrivere */
			chControllo = 'S';
		else chControllo = 'N';
		/* printf("PADRE per indice %d sta per mandare %c\n", i, chControllo); */
		if (!finito[i])
		{
			nw=write(pipe_pf[i][1], &chControllo, 1); 
			if (nw != 1)
				printf("PADRE non e' riuscito ad inviare nulla al figlio di indice %d \n", i);
		
		}
	}
/* printf("PADRE valore della funzione finitof=%d\n", finitof()); */
}

/* Attesa della terminazione dei figli */
for(i=0;i<N;i++)
{
   pidFiglio = wait(&status);
   if (pidFiglio < 0)
   {
      printf("Errore wait\n");
      exit(7);
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
}/* fine del main */