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

typedef struct{
    	int pidn;	//c1 nel testo, pid nipote
        char linea[250]; //c2 nel testo, linea
        int lunghezza; //c3 nel testo, lunghezza
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
   	
    int N;   							/* numero di caratteri e quindi numero di processi */
    int i, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    pipe_t *pipedFP;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/

	//CONTROLLI TIPICI

    /* numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;



    /*allocazione N pipe */
    if ((pipedFP=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	

	//CREAZIONE PIPE 
    for (i=0; i < N; i++) {
        if (pipe(pipedFP[i]) < 0)        {
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

			for(k=0; k < N; k++){
                close(pipedFP[k][0]);
                if(k!= i){
                    close(pipedFP[k][1]);
                }
            }

           
            pipe_t pipedNF;
      

            /* prima creiamo la pipe di comunicazione fra nipote e figlio */
		  	if (pipe(pipedNF) < 0)
                	{	
                        	printf("Errore nella creazione della pipe fra figlio e nipote!\n");
                        	exit(-2);
                	}

			if ( (pid = fork()) < 0)
			{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-3);
			}	
			if (pid == 0) 
			{
			/* codice del nipote */
			printf("Sono il processo nipote del figlio di indice %d e pid %d e sto per recuperare le linee del file %s\n", i, getpid(), argv[i+1]);
				
                /* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
				close(pipedFP[i][1]);
				/* Ridirezione dello standard input il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
				close(0);
				if (open(argv[i+1], O_RDONLY) < 0)
				{
                                printf("Errore nella open del file %s\n", argv[i+1]);
                                exit(-4);
                        	}
				/* ogni nipote deve simulare il piping dei comandi nei confronti del figlio e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
				close(1);
				dup(pipedNF[1]); 			
				/* ogni nipote adesso p o' chiudere entrambi i lati della pipe: il lato 0 non viene usato e il lato 1 viene usato tramite lo standard output */
				close(pipedNF[0]);
				close(pipedNF[1]);
				/* Ridirezione dello standard error su /dev/null (per evitare messaggi di errore a video) */
				close(2);
				open("/dev/null", O_WRONLY);
			
				/* Il nipote diventa il comando wc -1 */				
				execlp("sort", "sort", "-f", (char *)0);
				/* attenzione ai parametri nella esecuzione di wc: aolo -1 e terminatore della lista. */ 
				
				/* Non si dovrebbe mai tornare qui!!: ATTENZIONE avendo chiuso lo standard output e lo standard error NON si possono fare stampe con indicazioni di errori; nel caso, NON chiudere lo standard error e usare perror o comunque write su 2 */
				exit(-1); 
			}

            

            close(pipedNF[1]);
            s_occ s; 
            s.lunghezza = 0;
            cont = 0;

            while (read(pipedNF[0], &(s.linea[cont]), 1)){
                cont++;
            
                if(s.linea[cont - 1] == '\n'){
                    s.lunghezza = cont;  //salvo l'attuale lunghezza della riga, non so infatti se sarà l'ultima. Poi azzero il contatore.
                    cont = 0;
                }
                
            }
            
            //aspetto il nipote da cui prenderò il pid per poi salvarlo nella struct
            pid = wait(&status);
			
            ritorno=-1;
            if (pid < 0)
			{	
				printf("Errore in wait\n");
				exit(ritorno);
			}
			if ((status & 0xFF) != 0)
    				printf("Nipote con pid %d terminato in modo anomalo\n", pid);
    	

            s.pidn = pid;

            //mando la struct al padre
            write(pipedFP[i][1], &s, sizeof(s));

            //riporto la lunghezza effettiva
			exit( s.lunghezza-1);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
  
    /* Il padre chiude i lati delle pipe che non usa */
	for (i=0; i < N; i++)
		close(pipedFP[i][1]);

    /* Il padre recupera le informazioni dai figli in ordine di indice */
	s_occ s;
    for (i=0; i < N; i++)
	{
		/* il padre recupera tutti le strutture dai figli */
		read(pipedFP[i][0], &s, sizeof(s));
        s.linea[s.lunghezza-1]= 0;
		
        printf("Il figlio di indice %d ha letto la linea %s di lunghezza %d attraverso il nipote %d per il file %s\n", i, s.linea, s.lunghezza, s.pidn, argv[i+1]);
		
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
