// COSE STANDARD
    //vedi filescheletro.c
    

//CONTROLLI PARTICOLARI

       
    
    /* controllo singoli caratteri */
    for (i=0; i < N; i++)
        if (strlen(argv[ ]) != 1)
        { 
            printf("Errore nella stringa %s che non e' un singolo carattere\n", argv[]);
            exit(1);
        }

     /* controllo numero strettamente positivo minore di 255*/  
    H = atoi(argv[]);
  
    if (H <= 0 || H >= 255)  /* il numero deve essere strettamente positivo e strettamente minore di 255 */
        {
	        printf("Errore parametro %d\n", H);
	        exit(2); 
        }
    

//ALLOCAZIONE MEMORIA MALLOC

    /* allocazione N pipe */
    if ((p=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }




//APERTURA FILE

        
                    
    /* apre il file */
    if ((fdr = open(argv[1], O_RDONLY)) < 0)
    { 
        printf("Errore nella apertura del file %s\n", argv[1]);
        exit(-1);
    }

//CONTA LE OCCORRENZE

            /* conta le occorrenze del carattere assegnato */
            
            while(read(fdr, &c, 1) > 0)
            {
                if (c == argv[i+2][0])
                    cont++;
            }
            
//ATTESA DEL PADRE RIGA PER RIGA E POI FILE PER FILE

            /* Il padre recupera le informazioni dai figli: prima in ordine di strutture e quindi in ordine di indice */
            finito = 0; /* all'inizio supponiamo che non sia finito nessun figlio */
            while (!finito)
            {
                finito = 1; /* mettiamo finito a 1 perche' potrebbero essere finiti tutti i figli */
                for (i = 0; i < N; i++)
                {
                    /* si legge la struttura inviata  dal figlio i-esimo */
                    nr = read(piped[i][0], &S, sizeof(S));
                    if (nr != 0)
                    {
                        finito = 0;         /* almeno un processo figlio non e' finito */
                                                                                                                                                                             /* Nota bene: la stampa della linea con il formato %s NON ha problemi perche' il figlio ha passato una stringa */
                        printf("Il nipote con pid %d ha letto dal file %s nella riga %d questa linea:\n%s", S.pid_nipote, argv[(i * 2) + 1], S.numero_linea, S.linea_letta); /* NOTA BENE: dato che la linea contiene il terminatore di linea nella printf NON abbiamo inserito lo \n alla fine */
                    }
                }
            }

//ATTESA TERMINAZIONE DEI FIGLI   

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
                printf("Il  figlio  con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", pidFiglio, ritorno);
                }
        }
        exit(0);
    }

//per trasformare carattere in stringa:

sprintf