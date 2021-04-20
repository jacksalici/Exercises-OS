#/bin/sh

#controllo di passare almeno tre parametri
if test $# -lt 2
then 
	echo immettere almeno tre parametri
	exit 1
fi

#controllo sai un numero positivo
case $1 in 
*[!0-9]*) echo $1 non numerico positivo
	exit 4;;
*)	if test $1 -eq 0
	then echo $1 non positivo
	fi
;;
esac
H=$1


#controllo sai un numero positivo
case $2 in 
*[!0-9]*) echo $2 non numerico positivo
	exit 4;;
*)	if test $2 -eq 0
	then echo $2 non positivo
	fi
;;
esac
K=$2

#shifto tutto i parametri 
shift
shift


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
    esame170221_rec.sh $H $K $i $file 
done


echo FILE SORTED `wc -l < $file`
cat $file

rm $file