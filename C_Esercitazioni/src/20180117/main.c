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


//per segnali
int token = 1;

void scrivi()
{
    token = 1;
}

void salta()
{
    token = 0;
}

typedef struct
{
    int c2; /* valore linee */
    int c1;      /* indice figlio */
} s_occ;

int main(int argc, char *argv[])
{

    int *pid; /* array di pid */

    int N;                          /* numero di caratteri e quindi numero di processi */
    int fd;                         /* per open */
    int i, k;                       /* indici, i per i figli! */
    long int cont;                  /* per conteggio */
    char c[255];                    /* per leggere dal file (supponiamo bastino 255 caratteri*/
    pipe_t *piped;                  /* array dinamico di pipe */
    int pidFiglio, status, ritorno; /* variabili per wait*/
    s_occ s;                        //per salvare la struct corrente
    
    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 4)
    {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }

    N = argc - 3;

    /* controllo singolO  carattere penultimo param */
    if (strlen(argv[argc-2]) != 1)
        {
            printf("Errore nella stringa %s che non e' un singolo carattere\n", argv[argc - 2]);
            exit(1);
        }
    char Cx = argv[argc-2][0];

    /* controllo numero strettamente positivo minore di 255*/
    int H = atoi(argv[argc-1]);

    if (H <= 0 || H >= 255) /* il numero deve essere strettamente positivo e strettamente minore di 255 */
    {
        printf("Errore parametro %d\n", H);
        exit(2);
    }

    //ALLOCAZIONE MEMORIA MALLOC

    /* allocazione N pipe */
    if ((piped = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL)
    {
        printf("Errore allocazione pipe\n");
        exit(2);
    }

    /*allocazione array per i pid */
    if ((pid = (int *)malloc(N * sizeof(int))) == NULL)
    {
        printf("Errore allocazione pid\n");
        exit(3);
    }

    /* creo N pipe */
    for (i = 0; i < N; i++)
    {
        if (pipe(piped[i]) < 0)
        {
            printf("Errore nella creazione della pipe\n");
            exit(4);
        }
    }

    /* padre aggancia le due funzioni (scrivi e salta) che useranno i figli alla ricezione dei segnali inviati dal padre */
    signal(SIGUSR1, scrivi);
    signal(SIGUSR2, salta);

    printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);

    for (i = 0; i < N; i++)
    {
        /* creazione dei figli */
        if ((pid[i] = fork()) < 0)
        {
            printf("Errore nella fork\n");
            exit(5);
        }

        if (pid[i] == 0) /* figlio */
        {
            printf("Figlio %d con pid %d\n", i, getpid());

            /* chiude tutte le pipe che non usa */

            //schema pipeline: ogni figlio legge dalla pipe i-1 e scrive sulla i
            for (k = 0; k < N; k++)
            {
                if (k != i)
                    close(piped[k][1]);
                if (i == 0 || k != i - 1)
                    close(piped[k][0]);
            }

            /* apre il file */
            if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
            {
                printf("Errore nella apertura del file %s\n", argv[i + 1]);
                exit(-1);
            }

            /* eseguo compito figlio */

            cont = 0; //inizializzo a 0 il n delle linee stampate a video
            int occ = 0;  //n occorrenze del carattere nella linea corrente
            int j=0; //indice linea
            int n_linea=0; //per n linea corrente (mi serve solo da stampare a video)
            while (read(fd, &c[j], 1) > 0) //leggo fino alla fine il file associato
            {
                if (c[j]==Cx){
                    occ++; //controllo se ho trovato il carattere 
                }
                if (c[j] == '\n'){ //per ogni linea eseguo il seguente codice
                    n_linea++;
                    c[j]=0;    //trasformo il vettore linea in stringa
                    //tutti i figli, eccetto il primo, leggono la struct del figlio "precedente" come numero d'ordine
                    if (i != 0)
                    {
                        if (read(piped[i - 1][0], &s, sizeof(s)) != sizeof(s))
                        {
                            printf("Errore in lettura da pipeline");
                            exit(-1);
                        }

                        //a questo punto si controlla quale file ha il numero maggiore di occorrenze. Nel caso sia il file del figlio corrente, vengono sostituite le info della struct con quelle del figlio corrente
                        if (s.c2 < occ)
                        {
                            s.c2 = occ;
                            s.c1 = i;
                        }
                    }
                    else
                    {

                        //il figlio con i==0 salva le info sulla struct, senza fare controlli perchè è il primo figlio
                        s.c2 = occ;
                        s.c1 = i;
                    }

                    //tutti i figli scrivono quidni la struct per farla leggere al figlio successivo, assicurandosi che non ci siano errori in scrittura
                    if (write(piped[i][1], &s, sizeof(s)) != sizeof(s))
                    {
                        printf("Errore in scrittura da pipeline");
                        exit(-1);
                    }

                    //aspetto il segnale del padre
                    pause();

                    //solo un figlio stampa
                    if(token){
                        printf("LINEA %d: \"%s\", FIGLIO %i, pid %d\n", n_linea, c, i, getpid() );
                        cont++;
                    }
                    
                    
                    //terminato il lavoro per la linea corrente, metto a zero l'indice per la linea e il numero di occorrenze
                    j=0;
                    occ=0;
                }else{
                    j++; //se non trovo il terminare di stringhe semplicemente incremento l'indice
                }
            }

            
            //come da specifica si torna il numero di righe stampate
            exit(cont);
        }
    }

    /* padre */
    printf("Padre con PID: %d\n", getpid());

    /* chiude tutte le pipe che non usa */
    for (k = 0; k < N; k++)
    {
        close(piped[k][1]);
        if (k != N - 1)
        {
            close(piped[k][0]);
        }
    }

    //il padre legge il valore mandato dall'ultimo figlio, controllando di ricever etutti i byte che si aspetta e non di meno.
    for(k = 0; k < H; k++){
        if (read(piped[N - 1][0], &s, sizeof(s)) != sizeof(s))
        {
            printf("Errore in lettura da pipeline (padre)");
            exit(6);
        }

        //manda a tutti un sengale. "stampa" al figlio nella struct, "salta" a tutti gli altri
        for (i = 0; i < N; i++){
            if(i==s.c1){
                kill(pid[i], SIGUSR1);
            }else{
                kill(pid[i], SIGUSR2);
            }
        }

    }

    
    /* Attesa della terminazione dei figli */

    for (i = 0; i < N; i++)
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
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Il  figlio  con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
