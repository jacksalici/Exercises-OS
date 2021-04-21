#/bin/sh

if test $# -lt 3 #controllo di passare almeno due parametri
then 
	echo immettere almeno tre parametri
	exit 1
fi

#controllo se $1 è un numero strettamente positivo
case $1 in 
*[!0-9]*) echo $1 non numerico positivo
	exit 4;;
*)	if test $1 -eq 0
	then echo non positivo
	fi
;;
esac

L=$1
shift
#shifto cosi che tutti i parametri rimanenti debbano essere nomi ass dir

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

PATH=`pwd`:$PATH #aggiungo a $path la directory 
export PATH

file=/tmp/nomiAssoluti$$.txt #creo un file temporaneo


#eseguo il programma per ogni gerarchia
for i in $*
do
>$file
FCR.sh $L $i $file #Passo al file comandi ricorsivo i parametri richiesti.
echo NELLA GERARCHIA $i: FILE TROVATI: `wc -l <$file`
rm $file
done
#STAMPO RISULTATI

