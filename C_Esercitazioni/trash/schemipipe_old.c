/* 	caso 1: padre con N figli (comunicazione unilaterale)
	solo i figli comunicano scrivendo al padre */

/* caso 2: padre con N figli (comunicazione bilaterale)
	figli scrivono al padre e il padre scrive ai figli */

/* caso 3: padre con N figlio e ogni figlio ha un nipote 
	nipote scrive su stdout/figli e figli scrivono al padre */

/* caso y: comunicazione circolare 
	ogni figlio passa al successivo e l'ultimo passa al padre*/

/* caso x: comunicazione a ring
	innesco del padre ma poi il padre non c'entra più */

	/* figlio */
	if (pid == 0)  {
        printf("Figlio %d con pid %d\n", q, getpid());

		/* chiusura per schema a ring: */
		for (j=0; j < Q; j++){
			if(j != q)
				close(piped[j][0]);			// in lettura lascio aperta la pipe con lo stesso indice del figlio
			if(j != ((q+1) % Q))
				close(piped[j][1]);			// in scrittura lascio aperta la pipe ...
		}

		/* codice */

		read(piped[q][0], &ok, 1);			// leggo dalla pipe dell'indice del figlio
		write(piped[(q+1)%Q][1], &ok, 1); 	// scrivo sulla pipe "successiva" al figlio

		/* codice */
	}

	/* padre */
	printf("Sono il padre (pid: %d)\n", getpid());
	   
    /* chiude tutte le pipe che non usa */
	for(q = 1; q < Q; q++){
		close(piped[q][0]);			// tengo aperto la pipe[0] in scrittura per l'innesco 
		close(piped[q][1]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima scrittura
	}

	write(piped[0][1], &ok, sizeof(char));		// lancio l'innesco
	// sleep(1);
	close(piped[0][1]);							// posso chiudere la pipe iniziale di scrittura

	// NON chiudo la pipe in lettura!

/* caso x: comunicazione a ring
	innesco del padre che rimane nel ciclo di pipe */

	/* OBBLIGATORIO: allocazione Q+1 pipe */
    if ((piped=(pipe_t *)malloc((Q+1)*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	/* OBBLIGATORIO: creo Q+1 pipe */
    for(q=0; q <= Q; q++) {
        if (pipe(piped[q]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

		if (pid == 0) /* figlio */ {
			printf("Figlio %d con pid %d\n", q, getpid());

			/* chiusura per schema a ring: */
			for (j=0; j <= Q; j++){
				if(j != q)
					close(piped[j][0]);			// in lettura lascio aperta la pipe con lo stesso indice del figlio
				if(j != (q+1))
					close(piped[j][1]);			// in scrittura lascio aperta la pipe con indice subito dopo al figlio
			}


			read(piped[q][0], &ok, 1);			// leggo dalla pipe dell'indice del figlio per il via
			write(piped[q+1][1], &ok, 1); 	// scrivo sulla pipe "successiva" al figlio
		}

	/* padre */
    printf("Sono il padre (pid: %d)\n", getpid());
	   
    /* chiude tutte le pipe che non usa */
	for(q = 0; q <= Q; q++){
		if(q != 0)
			close(piped[q][1]);			// tengo aperto la pipe[0] in scrittura per l'innesco 
		if(q != Q)
			close(piped[q][0]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima scrittura
	}

	for(linea = 1; linea <= L; linea++) {
		printf("\nLinea %d\n", linea);
		write(piped[0][1], &ok, sizeof(char));		// lancio l'innesco a ogni giro
		read(piped[Q][0], &ok, 1);					// leggo dall'ultimo figlio
	}

	close(piped[0][1]);							// posso chiudere la pipe iniziale di scrittura
