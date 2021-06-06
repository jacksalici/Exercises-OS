/* 	caso 1: padre con N figli (comunicazione unilaterale)
	solo i figli comunicano scrivendo al padre */

						/*CHIUSURA PIPE:*/
						/* figlio */
							for (k = 0; k < N; k++)
							{
								close(p[k][0]);
								if (k != i)
									close(p[k][1]);
							}
						
						/*padre*/	
							for (k = 0; k < N; k++)
							{
								close(p[k][1]);
							}

/* caso 2: padre con N figli (comunicazione bilaterale)
	figli scrivono al padre e il padre scrive ai figli */

							
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

				/*figlio*/

							/* chiusura pipes inutilizzate */
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

				/* padre */

							/* chiusura pipe */
							for (i = 0; i < N; i++)
							{
								close(pipePadreFiglio[i][0]);
								close(pipeFiglioPadre[i][1]);
							}

/* caso 3: padre con N figlio e ogni figlio ha un nipote 
	nipote scrive su stdout/figli e figli scrivono al padre */

				/* codice nipote, che deve eseguire un comando shell*/
					
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
								printf("Sono il processo nipote del figlio di indice %d e pid %d e sto per recuperare l'ultima linea del file %s\n", j, getpid(), argv[j + 1]);
								/* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
								close(piped[j][1]);
								/* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec):  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
								close(0);
								if (open(argv[j + 1], O_RDONLY) < 0)
								{
									printf("Errore nella open del file %s\n", argv[j + 1]);
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
								execlp("wc", "wc", "-l", (char *)0);
								/* attenzione ai parametri nella esecuzione di wc: aolo -1 e terminatore della lista. */
					
								/* Non si dovrebbe mai tornare qui!!: ATTENZIONE avendo chiuso lo standard output e lo standard error NON si possono fare stampe con indicazioni di errori; nel caso, NON chiudere lo standard error e usare perror o comunque write su 2 */
								exit(-1);
							}

/* caso y: comunicazione circolare 
	ogni figlio passa al successivo e l'ultimo passa al padre*/

							//?

/* caso x1: comunicazione a ring
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

/* caso x2: comunicazione a ring
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
										close(piped[q][0]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima 	scrittura
								}

								for(linea = 1; linea <= L; linea++) {
									printf("\nLinea %d\n", linea);
									write(piped[0][1], &ok, sizeof(char));		// lancio l'innesco a ogni giro
									read(piped[Q][0], &ok, 1);					// leggo dall'ultimo figlio
								}

								close(piped[0][1]);							// posso chiudere la pipe iniziale di scrittura
