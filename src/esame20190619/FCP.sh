#/bin/sh
#CONTROLLO PARAMETRI

if test $# -lt 3 #controllo di passare almeno 2+1 parametri
then 
	echo immettere almeno tre parametri
	exit 1
fi

#controllo se $1 è un numero strettamente positivo
case $1 in 
*[!0-9]*) echo $1 non numerico positivo
	exit 2;;
*)	if test $1 -le 1 -o $1 -ge 255
	then echo non è nel range
    exit 3
	fi
;;
esac

#salvo il parametro in una var e shifto
H=$1
shift

#controllo che i param rimanenti siano nomi assoluti
for i in $*
do
		case $i in
        /*);;
        *)	 echo  $i deve essere un nome assoluto
            	exit 4
        ;; 
        esac
done


PATH=`pwd`:$PATH #aggiungo a $path la directory 
export PATH


#organizzo in più fasi la chiamata del file ricorsivo
for i in $*
do
    FCR.sh $H $i #Passo al file comandi ricorsivo i parametri richiesti.
done
