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

int mia_random(int n)
{
    int casuale;
    casuale = rand() % n;
    return casuale;
}

typedef int pipe_t[2];

typedef struct{
    			/*  */
	 			/*  */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
   	
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd, fcreato;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    
    pipe_t *pipePadreFiglio, *pipeFiglioPadre;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
    int l2 = 0;
    int * vlun; //vettore delle lunghezza
	//CONTROLLI TIPICI

    srand(time(NULL));

    /* apre il file */
    if ((fcreato = open("fcreato", O_CREAT | O_TRUNC | O_WRONLY)) < 0)
    {
        printf("Errore nella apertura del file %s\n", argv[1]);
        exit(-1);
    }

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 6)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;

      /* controllo numero strettamente positivo minore di 255*/  
    int H = atoi(argv[argc-1]);
  
    if (H <= 0 || H >= 255)  /* il numero deve essere strettamente positivo e strettamente minore di 255 */
        {
	        printf("Errore parametro %d\n", H);
	        exit(2); 
        }

	//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione pipe figli-padre */
    if ((pipeFiglioPadre = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL)
    {
        printf("Errore allocazione pipe padre\n");
        exit(3);
    }

    /* allocazione pipe padre-figli */
    if ((pipePadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL)
    {
        printf("Errore allocazione pipe padre\n");
        exit(4);
    }

    /* creazione pipe */
    for (i = 0; i < N; i++)
        if (pipe(pipeFiglioPadre[i]) < 0)
        {
            printf("Errore creazione pipe\n");
            exit(5);
        }

    /* creazione di altre N pipe di comunicazione/sincronizzazione con il padre */
    for (i = 0; i < N; i++)
        if (pipe(pipePadreFiglio[i]) < 0)
        {
            printf("Errore creazione pipe\n");
            exit(6);
        }

    if ((vlun = (int *)malloc(N * sizeof(int))) == NULL)
    {
        printf("Errore allocazione pipe padre\n");
        exit(4);
    }

    printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < N; i++) {
		/*  creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/*  chiude tutte le pipe che non usa*/
            for (j = 0; j < N; j++)
            {
                close(pipeFiglioPadre[j][0]);
                close(pipePadreFiglio[j][1]);
                if (j != i)
                {
                    close(pipeFiglioPadre[j][1]);
                    close(pipePadreFiglio[j][0]);
                }
            }

            /*  apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio*/
            int l = 0;
            char linea[255];
            cont = 0;

            while (read(fd, &linea[l], 1) > 0)
            {   
                //printf("%c, %d\n", linea[l], l);
                if (linea[l] == '\n'){
                   l++;
                    printf("lunghezza %d, sono il figlio %d\n", l, i);
                    
                    
                    //scrivo la lunghezza al padre
                    if (write(pipeFiglioPadre[i][1], &l, sizeof(l))!=4){
                        printf("Errrore in scrittura");
                    }
                    
                    //leggo la posizione
                    read(pipePadreFiglio[i][0], &l2, sizeof(l2));


                    //se la linea ja almeno quella lunghezza scrivo il carattere di quella posizione sul padre
                    if (l2<=l){
                        write(fcreato, &linea[l2], 1);
                        printf("ho stampato a video il %d esimo carattere, sono il figlio %d\n", l2, i); //stampa di debug
                        cont++;
                    }


                    l=0;
                }else{
                    l++;
                }
                
            }
            //ritrono il conto dei caratter s
            exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /*  chiude tutte le pipe che non usa */
    for (i = 0; i < N; i++)
    {
        close(pipePadreFiglio[i][0]);
        close(pipeFiglioPadre[i][1]);
    }

    //leggo e scrivo per ogni linea
    for (k=0; k < H; k++){
        /* legge dalle pipe i messaggi e li salva su un vettore di lunghezze cosi da averli pronti per poter scegliere csualmente una lunghezza*/

        for (i=0; i < N; i++){
            if (read(pipeFiglioPadre[i][0], &vlun[i], sizeof(int))!=4){
                printf("Errore lettura");
            }
            
        }


        //trovo il carattere casuale nella riga casuale
        l2 = mia_random(vlun[mia_random(N)]);


        //lo mando hai figli
        for (i = 0; i < N; i++)
        {
            write(pipePadreFiglio[i][1], &l2, sizeof(l2));
        }
    }



     //attesta terminazione figli   
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
