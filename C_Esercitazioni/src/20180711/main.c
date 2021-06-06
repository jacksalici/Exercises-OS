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

int token = 0;

void salta()
{
    token = 0;
}

void scrivi()
{
    token = 1;
}


//funzione per calcolare l'indice dell'elemento massimo di un vettore
int massimo(long int *pos, int N)
{
    int i;
    int i_max = 0;
    for (i = 0; i < N; i++)
    {
        if (pos[i] > pos[i_max])
        {
            i_max = i;
        }
    }
    return i_max;
}

int main(int argc, char *argv[])
{

    int *pid; /* array di pid */

    int N;                          /* numero di caratteri e quindi numero di processi */
    int fd;                         /* per open */
    int i, k;                       /* indici, i per i figli! */
    int cont;                       /* per conteggio */
    char c;                         /* per leggere dal file */
    pipe_t *piped;                  /* array dinamico di pipe */
    int pidFiglio, status, ritorno; /* variabili per wait*/
    long int *pos;
    long int p;
    char CZ;
    //CONTROLLI TIPICI

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 4)
    {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }

    N = argc - 2;

    if (strlen(argv[1]) != 1)
    {
        printf("Errore nel parametro\n");
        exit(2);
    }
    CZ=argv[1][0];

    //ALLOCAZIONE MEMORIA

    /* allocazione N pipe */
    if ((piped = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL)
    {
        printf("Errore allocazione pipe\n");
        exit(3);
    }

    /* allocazione array per i pid */
    if ((pid = (int *)malloc(N * sizeof(int))) == NULL)
    {
        printf("Errore allocazione pid\n");
        exit(4);
    }

    /* allocazione array per i posizioni */
    if ((pos = (long int *)malloc(N * sizeof(long int))) == NULL)
    {
        printf("Errore allocazione vettore posizione\n");
        exit(4);
    }

    /* padre aggancia le due funzioni (scrivi e salta) che useranno i figli alla ricezione dei segnali inviati dal padre */
    signal(SIGUSR1, scrivi);
    signal(SIGUSR2, salta);

    /* creo N pipe */
    for (i = 0; i < N; i++)
    {
        if (pipe(piped[i]) < 0)
        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
    }

    printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i = 0; i < N; i++)
    {
        /* OBBLIGATORIO: creazione dei figli */
        if ((pid[i] = fork()) < 0)
        {
            printf("Errore nella fork\n");
            exit(1);
        }

        if (pid[i] == 0) /* figlio */
        {
            printf("Figlio %d con pid %d\n", i, getpid());

            /* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
            for (k = 0; k < N; k++)
            {
                close(piped[k][0]);
                if (k != i)
                {
                    close(piped[k][1]);
                }
            }

            /* apre il file */
            if ((fd = open(argv[i + 2], O_RDONLY)) < 0)
            {
                printf("Errore nella apertura del file %s\n", argv[i + 2]);
                exit(-1);
            }
            p = 0;
            cont = 0;

            while (read(fd, &c, 1) > 0)
            {
                if (c == CZ)
                {
                    //se trova un carattere lo manda al padre e poi si ferma ad aspettare
                    write(piped[i][1], &p, sizeof(p));
                    cont++; //aumenta il contatore
                    pause();

                    //se il padre ha mandato il segnale collegato alla funzione "SCRIVI", manda su stdout il carattere e altre indicazioni richieste dal testo.
                    if (token)
                    {
                        printf("CARATTERE TROVATO %c, DAL FIGLIO DI ORDINE %d E PID %d, NELLA POSIZIONE %ld DEL FILE %s\n", c, i, getpid(), p, argv[i + 2]);
                    }
                }
                p++; // amumenta la posizione
            }

            exit(cont);
        }
    }

    /* padre */
    printf("Padre con PID: %d\n", getpid());

    /* chiude tutte le pipe che non usa */
    for (k = 0; k < N; k++)
    {
        close(piped[k][1]);
    }

    //finche non sono finiti tutti i figli leggere, un carattere alla volta per figlio
    int finito = 0;
    while (!finito)
    {
        finito = 1;
        for (i = 0; i < N; i++)
        {
            pos[i] = 0;

            if (read(piped[i][0], &pos[i], sizeof(long int)) > 0)
            {
                finito = 0;
            }
        }


        //calcola l'indice dell'elemento massimo. Solo al figlio collegato manderà il segnale scrivi      
        int max = massimo(pos, N);

        for (i = 0; i < N; i++)
        {
            if (i == max)
            {
                kill(pid[i], SIGUSR1);
            }
            else
            {
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
            exit(9);
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
