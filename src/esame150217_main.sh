#/bin/sh

#controllo di passare due parametri
if test $# != 2
then 
	echo immettere due parametri
	exit 1
fi

#controllo che il primo sia un nome assoluto
case $1 in
/*);;
*)	 echo il primo valore deve essere un nome assoluto
    	exit 2
;; 
esac


#controllo che il primo sia una directory navigabile
if test ! -d $1 
then
	echo il primo parametro deve essere una directory 
	exit 3
fi

#controllo sai un numero positivo
case $2 in 
*[!0-9]*) echo $2 non numerico positivo
	exit 4;;
*)	if test $2 -eq 0
	then echo non positivo
	fi
;;
esac

#salvo in path la directory
PATH=`pwd`:$PATH
export PATH

#creo un file temporaneo
file=/tmp/f150217$$.txt
>$file

#Passo al file comandi ricorsivo rispettivamente: la gerarchia su cui operare, il numero di righe che deve avere ciascun file e il nome del file temporaneo
esame150217_rec.sh $1 $2 $file

#stampo a video il file temporaneo per poi rimuoverlo.

cat $file
rm $file

