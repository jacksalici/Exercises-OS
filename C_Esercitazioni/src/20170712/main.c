#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>

typedef int pipe_t[2];

typedef struct{
    	int pidn; //pid del nipote, c1 nel testo
        int nlinea; //numero della linea, c2 nel testo
        char linea[250]; //linea corrente	
	 		
} s_occ;

int mia_random(int n)
{
    int casuale;
    casuale = rand() % n;
    casuale++;
    return casuale;
}



int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
   	
    int N;   							/* numero di caratteri e quindi numero di processi */
    int i, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    s_occ s; //per salvare la linea corrente
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = (argc - 1)/2;

    /* controllo numero strettamente positivo minore di 255*/
    
    for (i=0; i < N; i++) {
        int H=atoi(argv[i*2+2]);
        if (H <= 0) /* il numero deve essere    strettamente positivo */
        {
            printf("Errore parametro %d\n", H);
            exit(2);
        }
    }

    //ALLOCAZIONE MEMORIA MALLOC

    /* allocazione N pipe */
    if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }



    /* creo N pipe */
    for (i=0; i < N; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < N; i++) {
		
        /* creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

            for (k = 0; k < N; k++)
            {
                close(piped[k][0]);
                if (k != i)
                    close(piped[k][1]);
            }

          
            pipe_t p;
            /* prima creiamo la pipe "p" di comunicazione fra nipote e figlio */
            if (pipe(p) < 0)
            {
                printf("Errore nella creazione della pipe fra figlio e nipote!\n");
                exit(-2);
            }

            if ((pid = fork()) < 0)
            {
                printf("Errore nella fork di creazione del nipote\n");
                exit(-3);
            }
            if (pid == 0)
            { 
                /* codice del nipote */
                //per prima cosa genero il file casuale:
                srand(time(NULL));

                int ran = mia_random(atoi(argv[2 * i + 2]));
                char r[15] = {0};

                sprintf(r, "-%d", ran);

                
                printf("Sono il processo nipote del figlio di indice %d e pid %d e sto per recuperare le prime %d linee del file %s\n", i, getpid(), ran, argv[2*i + 1]);

               
                

                /* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
                close(piped[i][1]);
                
                
                /* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec):  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
                close(0);
                if (open(argv[i * 2 + 1], O_RDONLY) < 0)
                {
                    printf("Errore nella open del file %s\n", argv[i * 2 + 1]);
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
                execlp("head", "head", r, (char *)0);
               

                /* Non si dovrebbe mai tornare qui!!: ATTENZIONE avendo chiuso lo standard output e lo standard error NON si possono fare stampe con indicazioni di errori; nel caso, NON chiudere lo standard error e usare perror o comunque write su 2 */
                exit(0);
            }

            s.pidn = pid;

            close(p[1]);
            
            s.nlinea = 0;
            cont = 0;

            //leggo carattere per carattere la pipe col nipote 
            while (read(p[0], &(s.linea[cont]), 1)==1)
            {   
                printf("%c", s.linea[cont]);
                if (s.linea[cont] == '\n')
                {
                    s.linea[cont] = 0; //trasformo la riga in stringa
                    

                    write(piped[i][1], &s, sizeof(s)); //mando la struct al padre

                    ++(s.nlinea); //incremento il numero linea nella struct e ricomincio il ciclo
                    cont = 0;
                }else{
                    cont++;
                }
            }

            //aspetto il nipote per evitare errori
            pid = wait(&status);

            ritorno = -1;
            if (pid < 0)
            {
                printf("Errore in wait\n");
                exit(ritorno);
            }
            if ((status & 0xFF) != 0)
                printf("Nipote con pid %d terminato in modo anomalo\n", pid);

            

           
            //riporto il numero dell'ultima linea letta che rapprensenta di fatto il valore random generato
            exit(s.nlinea);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
    for (k = 0; k < N; k++)
    {
        close(piped[k][1]);
    }
    /* legge dalle pipe i messaggi o manda segnali?*/

    int finito = 0;

    /* Il padre recupera le informazioni dai figli: prima in ordine di strutture e quindi in ordine di indice */
    while (!finito){
        finito = 1;
        for (i = 0; i < N; i++){
            if (read(piped[i][0], &s, sizeof(s))>0){
                finito = 0; /* almeno un processo figlio non e' finito */

                printf("FIGLIO %d, LINEA N %d; \"%s\", pid nipote %d\n", i, s.nlinea, s.linea, s.pidn);
            }
        }

    }    


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
