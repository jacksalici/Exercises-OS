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

    int X;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int x, j;     						/* indici, x per x figli! */
    int cont = 0;     						/* per conteggio */
    pipe_t *piped, p;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    int FCreato; /*per il figlio da creare*/
    char buffer[250]; //dove salvare la linea

    //CONTROLLI TIPICI

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 2)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    X = argc - 1;


    //per prima cosa creo un file con il mio nome nella cartellla /tmp/

    if ((FCreato = open("/tmp/giacomosalici", O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0)
    {
        printf("Errore nella creazione del file %s\n", argv[1]);
        exit(2);
    }

    //creo un figlio speciale, che deve eseguire un comando shell*/

    /* prima creiamo la pipe "p" di comunicazione fra padre e figlio speciale controllando possibili errori*/

   
    
       
    if (pipe(p) < 0)
    {
        printf("Errore nella creazione della pipe fra figlio e figlio speciale!\n");
        exit(3);
    }

    //creo il processo del figlio speciale

    if ((pid = fork()) < 0)
    {
        printf("Errore nella fork di creazione del figlio speciale\n");
        exit(4);
    }
    if (pid == 0)
    {
        /* codice del figlio speciale */
       
       
        /* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec)*/
        close(0);

        //apro il file passato come primo carattere
        if (open(argv[1], O_RDONLY) < 0)
        {
            printf("Errore nella open del file %s\n", argv[1]);
            exit(-1);
        }


        /* ogni figlio speciale simula il piping dei comandi chiudendo lo standard output e usando la primitiva dup sul lato di scrittura della propria pipe */
        close(1);
        dup(p[1]);
        
        /* ogni figlio speciale adesso puo' chiudere entrambi i lati della pipe: il lato 0 non viene usato e il lato 1 viene usato tramite lo standard output */
        close(p[0]);
        close(p[1]);
       
        /* Ridirezione dello standard error su /dev/null (per evitare messaggi di errore a video) */
        close(2);
        open("/dev/null", O_WRONLY);

        /* Il figlio speciale diventa il comando wc -l */
        execlp("wc", "wc", "-l", (char *)0);
       
        /* Non si dovrebbe mai tornare qui!!: */
        exit(-1);
    }

    //chiudo la pipe con il figlo speciale
    close(p[1]);
    //inizializzo il contatore per leggere dalla pipe del figlio speciale
    cont=0;
    char righe[10]; //ipotizziamo che il file abbia massimo 9 caratteri + il terminatore
    

    //leggo tutti i caratteri (numeri) della pipe
    while (read(p[0], &(righe[cont]), 1))
    {
        cont++;
    }

    //trasformo il vettore di caratteri in stringhe e poi trasformo la stringa in un intero
    righe[cont] = 0;
    int n_righe= atoi(righe);
    printf("Il figlio speciale ha contato %d righe.\n", n_righe); //stampa di debug

    //aspetto che termini il figlio speciale per non avere problemi in fase di terminazione degli altri figli
    pid = wait(&status);

    ritorno = -1;
    if (pid < 0)
    {
        printf("Errore in wait\n");
        exit(ritorno);
    }
    if ((status & 0xFF) != 0)
        printf("Il figlio speciale con pid %d terminato in modo anomalo",
               pid);

    //salvato il numero di riche del primo figlio alloco la memoria per gli altri figli



    /* allocazione X pipe descripotros */
    if ((piped=(pipe_t *)malloc(X*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(5); 
    }

    /*  creo X pipe padre figlio */
    for (x=0; x < X; x++) {
        if (pipe(piped[x]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), X);//stampa di debug


    for (x=0; x < X; x++) {

		/* creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(6);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", x, getpid());

			/* chiude tutte le pipe che non usa*/
			for(j = 0; j < X; j++){
                close(piped[j][0]);
                if (j != x)
                    close(piped[j][1]);
            }

			/*  apre il file */
            if ((fd = open(argv[x+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[x+1]);
                exit(-1);
            }

			
            /* eseguo codice figlio e eventuale figlio speciale */
            cont=0;
            int indice = 0;
            while (read(fd, &buffer[indice], 1) > 0)
            {
                //leggo tutto il file contando i caratteri per riga
                if (buffer[indice]=='\n'){
                    indice++;
                    //ogni volta che arrivo alla fine della riga mando al padre la lunghezza...
                    write(piped[x][1], &indice, 4);

                    //...e poi la linea
                    write(piped[x][1], buffer, indice);     

                    cont=indice; //salvo la lunghezza dell'ultima riga e poi inizializzo la lunghezza
                    indice=0;
                }else
                {
                        indice++; //incrememento l'attuale lunghezza se non è ancora finita la linea
                }
            }

            //esco con la lunghezza dell'ultima linea meno il terminatore di stringa
            exit(cont-1);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
	for(j = 0; j < X; j++){
        close(piped[j][1]);
    }

    /* Il padre recupera le informazioni dai figli rispettando prima l'ordine di linea e poi l'ordine di indice */
    int i;
    for(j = 0; j < n_righe; j++){
        for(x=0; x < X; x++){
            

            //leggo la lunghezza di ogni riga e poi la riga, infine riporto la linea sul file
            read(piped[x][0], &i, 4);
            read(piped[x][0], buffer, i );

            write(FCreato, buffer, i);
        }
    }


   /* Attesa della terminazione dei figli e della lunghezza della loro ultima riga */
        
    for(x=0;x < X;x++) {
    	pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore wait\n");
	        exit(9);
    	}
            
        if ((status & 0xFF) != 0)
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  figlio  con  pid=%d  ha  ritornato  %d (lunghezza effettiva ultima riga)(se  255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
