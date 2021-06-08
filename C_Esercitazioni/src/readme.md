# Indice Esercizi

*in aggiornamento continuo, man mano che svolgo i vecchi esami*  


20160608

> **padre-figlio e figlio-padre** ogni figlio manda la lunghezza della riga, il padre manda una posizione casuale fra tutte le lunghezze delle righe, se esiste per il singolo figlio stampo su un file creato


20160713

> **padre-figlio e figlio-padre** dopo che il padre dice a ogni figlio un divisore, mandiamo da ogni figlio i caratteri multipli

20160909

> **pipeline** si trasmette un vettore contenente il numero di occorrente del carattere assegnato ad ogni figlio. il padre infine riordina con il *bubblesort* il vettore e stampa tutto [non commentato bene, manca qualche controllo] 

20170526

> **pipeline:** ogni figlio è associato ad un file, e ci si passa una struct con il numero massimo e la somma di delle occorrenze di un carattere.


20170614

> **padre-figlio e figlio-padre** ogni figlio cerca un carattere e, ogni volta che trova un'occorrenza chiede al padre come sostituirlo

20170712

> **nipote** [difficile e confusionario] il nipote fa la head e il figlio manda riga per riga al padre.

20170913

> **figlio-padre** ogni figlio selezione l'ultima linea del file associato e al manda al padre che la scrive su un nuovo file. [non commentato benissimo]

20180117

> **pipeline e segnali** [molto lungo, forse non troppo chiaro] per ogni linea di ogni file si passa avanti una struct con il maggior numero di occcorrenze di un carattere e di che figlio stiamo parlando. Il padre riceve una struct per linea e manda sengali di stampa della riga al figlio con il maggior numero di occorrenze del carattere

20180601

> **figlio-padre** ogni figlio selezione una certa linea del file associato e al manda al padre che la scrive a video [non commentato benissimo]

20180711

> **figlio-padre con segnali** ogni figlio selezione un certo carattere, manda al padre la sua posizione. Il padre manderà un segnale di stampa al figlio con posizione *massima*

20180912

> **nipote** il nipote trasforma in maiuscolo tutti i caratteri di un file, il figlo sostiutisce i numeri con spazi. Ognuno ritorna al proprio genitore il numero dei caratteri trovati diviso 256 castato int.

20190116

> **figlio-padre con segnali** ogni figlio seleziona la lunghezza delle linee e la manda al padre. le linee lunghe uguali vengono mandate al padre che le stampa dopo aver mandato un sengale

20190213

> **pipeline:** ogni figlio è associato ad un file, e ci si passa una struct con il numero di linee massimo.

20190531

> **nipote** il nipote ordina il file, il figlio ritorna la lunghezza dell'ultima riga.

20190619

> **figlio-padre con segnali** ogni figlio selezione il primo carattere di ogni riga del suo file. Il padre manderà un segnale di stampa della linea al figlio con valore *massima*

20190911

> **coppia di figli** il figlio dispari crea un file e ci mette il file maggiore fra ogni coppia di caratteri letta dai file associati al figlio pari e dispari.

20200212

> **coppia di figli** i figli leggono certi caratteri a seconda se sono pari o dispari e mandano al padre una struct con il risultato dei conteggi


20200610

> **figlio-padre** ogni figlio manda al padre le linee che rispettano certi paramentri [non commentato benissimo]

20200617

> **figlio-padre** ogni figlio legge dal proprio file una parte ben specifica e la manda al padre che la scrive su un file creato. 

20200715

> **figlio-padre** ogni figlio legge dal file la sua linea e manda al padre il secondo e il pensultimo carattere in una struct. 

20200909

> **figlio-padre** ogni figlio conta le occorrenze e le manda al padre. *Easy Peasy, buona base di partenza*

20210121

> **schema ring** il padre serve solo da innesco. i figli leggono un carattere ad ogni giro 
