#/bin/sh

#controllo di passare almeno due parametri
if test $# -lt 2
then 
	echo immettere almeno due parametri
	exit 1
fi

#controllo sai un numero positivo
case $1 in 
*[!0-9]*) echo $1 non numerico positivo
	exit 4;;
*)	if test $1 -eq 0
	then echo non positivo
	fi
;;
esac

#salvo $1 nella var primo e shifto i parametri
primo=$1
shift


#controllo che i param rimanenti siano  nomi assoluti
for i in $*
do
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
done



#salvo in path la directory
PATH=`pwd`:$PATH
export PATH

#creo un file per contare globalmente i file
>/tmp/f$$.txt
file=/tmp/f$$.txt

#Passo al file comandi ricorsivo i parametri
for i in $*
do
 	esame110418_rec.sh $primo $i $file
done

echo FILE CREATI `wc -l < $file`

for i in `cat $file`
do
	echo FILE $i
	cat $i
	echo "########"
	rm $i
done

