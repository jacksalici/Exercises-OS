#/bin/sh

#controllo di passare almeno due parametri
if test $# -lt 2
then 
	echo immettere almeno due parametri
	exit 1
fi


#controllo che il primo sia un nome assoluto

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
ultimo=$i
fi
done



#salvo in path la directory
PATH=`pwd`:$PATH
export PATH

#creo un file per contare globalmente i file
>/tmp/f$$.txt
>/tmp/f2$$.txt
file1=/tmp/f$$.txt
file2=/tmp/f2$$.txt

#Passo al file comandi ricorsivo rispettivamente: la gerarchia su cui operare, il numero di righe che deve avere ciascun file e il nome del file temporaneo
j=1
for i in $*
do
if test $j -ne $#
then

 	esame050419_rec.sh $i $ultimo $file1 $file2
	j=`expr $j + 1`

fi

done

echo N FILE TROVATI: `wc -l < $file1`
e=1
for i in `cat $file1`
do
		echo FILE $i
		echo CARATTERI `tail -1 $file2 | head -$e`
		e=`expr $e + 1` 

		echo vuoi ordinare il file? s/n
read s

	if test $s = s
	then 
			echo `sort -f $i`
	else
		echo "sorting aborted"
	fi
done
		
	




