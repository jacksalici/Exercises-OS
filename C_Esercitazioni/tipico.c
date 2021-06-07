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

