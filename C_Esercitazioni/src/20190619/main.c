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

//variabile token per far si che il figlio scriva o meno
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
int massimo(char *car, int N)
{
    int i;
    int i_max = 0;
    for (i = 0; i < N; i++)
    {
        if (car[i] > car[i_max])
        {
            i_max = i;
        }
    }
    return i_max;
}

int main(int argc, char *argv[])
{

    int *pid;                       /* array di pid */
    int N;                          /* numero di file e quindi numero di processi */
    int fd;                         /* per open */
    int i, k, l;                       /* indici, i per i figli! */
    int cont;                       /* per conteggio caratteri letti */
    
    pipe_t *piped;                  /* array dinamico di pipe */
    int pidFiglio, status, ritorno; /* variabili per wait*/
    char *car;                  /* array dinamico di posizioni */
   
    char linea[250]; //per salvare la linea corrente
    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 3)
    {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }

    N = argc - 1;

   

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
    if ((car = (char *)malloc(N)) == NULL)
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
        /* creazione dei figli */
        if ((pid[i] = fork()) < 0)
        {
            printf("Errore nella fork\n");
            exit(1);
        }

        if (pid[i] == 0) /* figlio */
        {
            printf("Figlio %d con pid %d\n", i, getpid());

            /*  chiude tutte le pipe che non usa (scegli schema chiusura!) */
            for (k = 0; k < N; k++)
            {
                close(piped[k][0]);
                if (k != i)
                {
                    close(piped[k][1]);
                }
            }

            /* apre il file */
            if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
            {
                printf("Errore nella apertura del file %s\n", argv[i + 1]);
                exit(-1);
            }
            
            cont = 0;
            l=0;
            while (read(fd, &linea[l], 1) > 0)
            {
                if (linea[l]=='\n')
                {
                    linea[l]=0;
                    //arrivato a fine linea crea una stringa e manda al padre il primo carattere
                    write(piped[i][1], &linea[0], 1);
                   
                    pause();

                    //se il padre ha mandato il segnale collegato alla funzione "SCRIVI", manda su stdout il carattere e altre indicazioni richieste dal testo.
                    if (token)
                    {
                        printf("IL FIGLIO DI ORDINE %d E PID %d, HA IL CARATTERE MASSIMO %c COME PRIMO CARATTERE DELLA STRINGA %s\n", i, getpid(), linea[0], linea);
                         cont++; //aumenta il contatore
                    }
                    l=0;
                }else{
                l++; // amumenta la posizione
                }
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
            car[i] = 0;

            if (read(piped[i][0], &car[i], 1) > 0)
            {
                finito = 0;
            }
        }

        //calcola l'indice dell'elemento massimo. Solo al figlio collegato manderà il segnale scrivi
        int max = massimo(car, N);

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
