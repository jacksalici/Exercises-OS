#/bin/sh

#controllo di passare almeno due parametri
if test $# -lt 2
then 
	echo immettere almeno due parametri
	exit 1
fi

#controllo che il primo sia un nome assoluto
case $1 in
/*);;
*)	 echo il primo valore deve essere un nome assoluto
    	exit 2
;; 
esac

G=$1
shift

#shifto tutto i parametri 

for i in $*
do
		case $i in
			*[!/]*);;
			*)	 echo $i deve essere relativo semplice
			   	exit 3
			;; 
		esac

done

#salvo in path la directory
PATH=`pwd`:$PATH
export PATH

#creo un file temporaneo
file=/tmp/$$.txt
>$file


#Passo al file comandi ricorsivo rispettivamente: 
FCR.sh $file $G $*


echo DIR TROVATE `wc -l < $file`
cat $file

rm $file