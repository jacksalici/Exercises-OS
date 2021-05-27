	#NUMERO PARAMETRI STRETTO


case $# in     #controllo che i parametri siano esattamente 4
 0|1|2|3) echo Errore: pochi argomenti >&2 
	exit 1;;
 4)     ;; #4 parametri è il numero giusto
 *) echo Errore: troppi argomenti >&2 exit 1;;
esac 

	#NUMERO PARAMETRI LASCO


if test $# -lt 2 #-le -ge -gt #controllo di passare almeno due parametri
then 
	echo immettere almeno due parametri
	exit 1
fi

	#CONTROLLO NOME ASSOLUTO E DIRETTORIO 


case $1 in     #controllo che il nome sia assoluto e la directory navigavile 
/*)   	if test ! -d $1 -o ! -x $1 
	then echo $1 non direttorio o non accessibile >&2 
	exit 2; 
	fi;; 
*) echo argomento sbagliato: $1 non assoluto >&2 
exit 3; ; 
esac 

#controllo che tutti i parametri, eccetto lultimo che salverò in LAST, siano nomi assoluti e directory navigabili
j=1

for i in $*
do
if 
	test $j -ne $#
then
	case $i in
		/*)		
				if test ! -d $i -o ! -x $i
				then 
					echo $i deve essere una directory navigabile
					exit 3
				fi
				;;
		*)		echo $i deve essere un nome assoluto
	    		exit 2
				;; 
		esac
	j=`expr $j + 1`

else 
LAST=$i
fi
done

#controllo che i param rimanenti siano nomi assoluti
for i in $*
do
		case $i in
        /*);;
        *)	 echo  $i deve essere un nome assoluto
            	exit 3
        ;; 
        esac
done

	#CONTROLLO NOME ASSOLUTO E FILE

#controllo che il nome sia assoluto e sia un file
case $2 in    
/*)   	if test ! -f $2    
	then     echo $2 non file >&2 
	exit 4 fi;; 
	*)    echo argomento sbagliato: $1 non assoluto >&2 
	exit 5;; 
esac




	#CONTROLLO NOME RELATIVO SEMPLICE

case $3 in #controllo che $3 sia relativo semplice
	*/*) 	echo argomento sbagliato: $3 nome non relativo semplice >&2
		exit 6;; 
	*) ;; 
esac #in questo caso NON si può controllare né che sia un file né che sia un direttorio!




	#CONTROLLO NUMERICO


#controllo se $2 è un numero strettamente positivo
case $2 in 
*[!0-9]*) echo $2 non numerico positivo
	exit 4;;
*)	if test $2 -eq 0
	then echo non positivo
	fi
;;
esac


	#CONTROLLO CARATTERE


case $1 in #controllo che il primo sia un carattere
?) ;;
*)	echo $1 deve essere un carattere
;;
esac




	#CHIAMATA RICORSIVA

#organizzo in più fasi la chiamata del file ricorsivo
for i in $*
do
    FCR.sh $H $K $i $file 
done


	#FAC SIMILE FCP.sh

#/bin/sh
#CONTROLLO PARAMETRI

PATH=`pwd`:$PATH #aggiungo a $path la directory 
export PATH

file=/tmp/$$.txt #creo un file temporaneo
>$file

FCR.sh #PARAM #Passo al file comandi ricorsivo i parametri richiesti.

#STAMPO RISULTATI

rm $file #rimuovo il file temporaneo









	#FAC SIMILE FCR.SH

#!/bin/sh
#CAMBIO DIRECTORY
cd $1

for i in * #controllo tutti gli elementi della directory corrente
do
	if test -f $i -a -r $i #mi assicuro che l'elemento corrente sia un file leggibile
	then
		#CONTROLLI
	fi	
done


for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -r $k -a -d $k #accertandomi che sia una cartella e sia leggibile
	then
		FCR.sh #PARAM
	fi
done 




	#CONTA LIVELLI

#file comandi ricorsivo che torna il numero totale di livelli 
#contati fino a quel punto

cd $1
#il primo livello verra' contato come livello 1
conta=`expr $2 + 1`  #nota bene questa variabile conta NON e' quella dello script principale, ma e' una variabile conta LOCALE a questo script
for i in *
do
	if test -d $i -a -x $i
	then 	
		FCR.sh `pwd`/$i $conta $3 $4
		ret=$?
                read prec < /tmp/tmpContaLivelli #leggiamo dal file temporaneo il valore del numero di livello
		if test $ret -gt $prec		 #se il valore calcolato in ricorsione e' maggiore, allora
		then
		echo $ret > /tmp/tmpContaLivelli #aggiorniamo il numero di livello raggiunto
		fi
	fi
done
if test $3 = B
then
	if test $4 -eq $conta #se nella seconda fase sono in una directory che corrisponde al livello cercato, dobbiamo stampare le info richieste
	then
		echo  Adesso visualizzo il contenuto della directory `pwd` che fa parte del livello $4
		ls -l #se dovessimo visualizzare tutte le info inclusi i file nascosti sarebbe ls -la oppure inclusi i file nascosti a parte . e .. sarebbe ls -lA
	fi 
fi 
exit $conta
