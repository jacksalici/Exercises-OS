#FILE E DIRECTORY
pwd #percorso

ls #disponde le cartelle
	-l #long
	-f #full
	-d #directory info
	-a #all
	-A #all but w/out .. & .
	-t #order by time
	-r #reverse order
	-F #file e directory
	-R #recursive
	-i #inunber


#STAMPA
cat 
echo
more

#OPERAZIONI SUI FILE

mv #sposta e rinomina

rm #remove
	-i #interactive
	-r #recursive * cartelle

ln #link
	-s #software

touch #rinnova l'ultima modifica

#COMMANDS

which #where is a command
whereis #same
id #show UID and GID

#USER AND RIGHTS

who #utenti collegati
w #quite the same
chmod #modifica i diritti
#[u g o a] [+ -] [rwx]

#VARIABILI
export #per esportare variabili
env #per visualizzare var d'ambiente
unset #elimina variabile

#PROCESSI
ps #processi in esec
	-f #full, con anche altri dettagli
	-l #ancora piu dettagli
	-e #tutti i processi attivi



#FILTRI

sort 
	-c -C #check maiuscolo riporta come ritorno
	-i #winthout non printables
	-f #ignora minuscole maiuscole
	-r #reverse
	-u #delete doubles
	
grep
	-n #numero
	-i #case insensitive
	-v #per mostrare solo cio che NON segue il pattern
	'^lettera' #inizio frase
	'lettera$' #fine frase
	"\." #per il punto
	#ritorna 0 se trova almeno un pattern, 1 se non trova nulla

wc	#con nome del file normalmente, senza con  ridirezione
	-c #conta caratteri
	-w #parole
	-l #lines


head #mostra le prime num righe
	-num

tail #mostra le ultime -num righe

rev #ribalta cio che gli viene ridiretto

basename #output solo il nome base di un file cartelle

tee 

 # per redirigere tutto: ls -l z* p* > file3  2>&1

date

diff




expr

eval

$? #valore di ritorno ultimo carattere

shift
set

#shabang  #!/bin/sh