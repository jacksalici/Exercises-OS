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

#controllo se $2 è un numero positivo
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
file=/tmp/$$.txt
>$file

#Passo al file comandi ricorsivo rispettivamente.
FCR.sh $1 $2 $file

#stampo a video il file temporaneo per poi rimuoverlo.
echo FILE TROVATI
cat $file

valori=

#richiedo un numero per ciascun file trovato:
for z in `cat $file`
do
	echo $z numero?
	read num
	valori="$valori $z $num"
done


#stampo i risultati e rimuovo il file
echo VALORI PASSABILI:
echo $valori
rm $file

