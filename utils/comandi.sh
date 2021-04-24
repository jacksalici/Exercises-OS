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

cat

echo

mv

rm #remove
	-i #interactive
	-r #recursive * cartelle

ln link
	-s #software

#COMMANDS

which #where is a command
whereis #same
id #show UID and GID

#USER AND RIGHTS

who #utenti collegati
w #quite the same


export #per esportare variabili
env #per visualizzare var d'ambiente
unset #elimina variabile


touch

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
	-c
	-w
	-l

head
	-num

tail

rev #ribalta cio che gli viene ridiretto

tee

 # per redirigere tutto: ls -l z* p* > file3  2>&1

date

diff

chmod
	[u g o a] [+ -] [rwx]

#shabang  #!/bin/sh

expr

eval

$? #valore di ritorno ultimo carattere

shift
set

