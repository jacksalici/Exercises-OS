#/bin/sh

#controllo di passare almeno due parametri
if test $# -lt 2
then 
	echo immettere almeno due parametri
	exit 1
fi

#controllo che il primo sia un nome assoluto
case $1 in
			*[!/]*);;
			*)	 echo il primo valore deve essere relativo semplice
			   	exit 2
			;; 
		esac

F=$1
shift

#shifto tutto i parametri 


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

#salvo in path la directory
PATH=`pwd`:$PATH
export PATH

#creo un file temporaneo
file=/tmp/$$.txt
>$file

for i in $*
do
    esame070417_rec.sh $F $i $file 
done


echo FILE SORTED `wc -l < $file`
for j in `cat $file`
do
echo "- ---------------------"
echo "-    FILE NAME:: $j"
echo "- FIRST STRING:: `head -1 $j`"
echo "-  LAST STRING:: `tail -1 $j`"

rm $j
done

rm $file