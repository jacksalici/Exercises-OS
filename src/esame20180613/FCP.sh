#/bin/sh

if test $# -lt 4 #controllo di passare almeno quattro parametri
then 
	echo $* immettere almeno quattro parametri
	exit 1
fi

case $1 in     #controllo che il primo par sia relativo semplice e la directory navigavile 
*[!/]*)   #	if test ! -d $1  
	#then echo $1 non direttorio o non accessibile >&2 
	#exit 2 
	#fi
    ;; 
*) echo argomento sbagliato: $1 non relativo >&2 
exit 3;; 
esac 

D=$1
shift 

case  $1 in #controllo che il nuovo primo parametro sia numerico
*[!0-9]*) echo $1 non numerico positivo
	exit 4;;
*)	if test $1 -eq 0
	then echo non positivo
	fi
;;
esac

Y=$1
shift

#controllo che i param rimanenti siano nomi assoluti
for i in $*
do
		case $i in
        /*);;
        *)	 echo  $i deve essere un nome assoluto
            	exit 5
        ;; 
        esac
done



PATH=`pwd`:$PATH #aggiorno a $path la directory 
export PATH



for i in $*
do
    file=/tmp/$$.txt #creo un file temporaneo
    > $file
    FCR.sh $D $Y $i $file #Passo al file comandi ricorsivo i parametri richiesti per ogni gerarchia
    #STAMPO RISULTATI
    echo FILE TROVATI: `wc -l < $file`
     #rimuovo il file temporaneo

    rm $file

done



