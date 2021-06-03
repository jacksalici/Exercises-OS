/* 	caso 1: padre con N figli (comunicazione unilaterale)
	solo i figli comunicano scrivendo al padre */

/* caso 2: padre con N figli (comunicazione bilaterale)
	figli scrivono al padre e il padre scrive ai figli */

/* caso 3: padre con N figlio e ogni figlio ha un nipote 
	nipote scrive su stdout/figli e figli scrivono al padre */

/* caso y: comunicazione circolare 
	ogni figlio passa al successivo e l'ultimo passa al padre*/

/* caso x: comunicazione a ring
	innesco del padre */

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
	
