#/bin/sh

#controllo di passare almeno due parametri
if test $# -lt 2
then 
	echo immettere almeno due parametri
	exit 1
fi

#controllo che il primo sia un carattere
case $1 in 
?) ;;
*)	echo $1 deve essere un carattere
;;
esac

#salvo $1 nella var primo e shifto i parametri
C=$1
shift


#controllo che siano nomi assoluti
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

file=/tmp/nomiAssoluti
>$file
#Passo al file comandi ricorsivo rispettivamente:
for i in $*
do
 	esame030620_rec.sh $C $i $file
done

echo DIRECTORY TROVATE `wc -l < $file` >/dev/tty


for i in `cat $file`
do
	echo DIRECTORY $i >/dev/tty
	echo visualizzare contenuto? >/dev/tty
	read elena 


    if test $elena = s
    then 
    echo `ls -a $i`
    fi
	
done

rm $file

