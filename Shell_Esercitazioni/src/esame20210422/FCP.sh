#/bin/sh
#CONTROLLO PARAMETRI


if test $# -lt 4 #controllo di passare almeno due+due parametri
then 
	echo immettere almeno 4 parametri
	exit 1
fi

#controllo se $1 è un numero strettamente positivo
case $1 in 
*[!0-9]*) echo $1 non numerico positivo
	exit 2;;
*)	if test $1 -eq 0
	then echo non strettamente positivo
        exit 3
	fi
;;
esac

#salvo il primi due paramentri in N e string, poi shifto due volte per rimuoverli dalla lista dei parametri
N=$1
string=$2
shift
shift

#controllo che i param rimanenti siano nomi assoluti
for i in $*
do
		case $i in
        /*);;
        *)	 echo  $i deve essere un nome assoluto
            	exit 4
        ;; 
        esac
done

file=/tmp/completeNames #creo un file temporaneo
>$file

PATH=`pwd`:$PATH #aggiungo a $path la directory e esporto
export PATH


#organizzo in più fasi la chiamata del file ricorsivo
for abs in $*
do
    FCR.sh $abs $string $file #Passo al file comandi ricorsivo i parametri richiesti.
done

#STAMPO RISULTATI
echo DIRECTORY TROVATE: `wc -l < $file` > /dev/tty


#se il risultato è maggiore o uguale a N, procedo con l'interazione con l'utente, che chiamo Giacomo
if test `wc -l < $file` -ge $N 
then

echo Giacomo, inserisci un numero compreso fra 1 e $N > /dev/tty #stampo su std output
read M

#controllo se $M è un numero compreso fra 1 e $N
case $M in 
*[!0-9]*) echo $M non numerico positivo
	exit 5
    rm $file #rimuovo il file temporaneo prima di uscire
    ;;
*)	if test $M -lt 1 -o $M -gt $N
	then echo non è nel range
        rm $file #rimuovo il file temporaneo prima di uscire
        exit 6
	fi
;;
esac
#stampo a video la lina $M esima
echo DIR SELEZIONATA: `head -$M $file | tail -1` > /dev/tty #stampo su std output
fi

rm $file #rimuovo il file temporaneo
