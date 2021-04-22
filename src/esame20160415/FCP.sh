#/bin/sh
#CONTROLLO PARAMETRI
if test $# -lt 3 #controllo di passare almeno tre parametri
then 
	echo immettere almeno tre parametri
	exit 1
fi

#controllo che tutti i parametri, eccetto lultimo che salverò in LAST, siano nomi assoluti e directory navigabili
j=1
ger=
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
    ger="$ger $i"
else
#controllo che l'ultimo sia positivo
case $i in 
*[!0-9]*) echo $i non numerico positivo
	exit 4;;
*)	if test $i -eq 0
	then echo non positivo
    exit 5
	fi
;;
esac 
X=$i
fi
done

PATH=`pwd`:$PATH #aggiungo a $path la directory 
export PATH






PATH=`pwd`:$PATH #aggiungo a $path la directory 
export PATH
file=/tmp/$$.txt #creo un file temporaneo
    >$file

#organizzo in più fasi la chiamata del file ricorsivo
for i in $ger
do
    
    FCR.sh $X $i $file #Passo al file comandi ricorsivo i parametri richiesti.
    
done


#STAMPO RISULTATI
echo dir trovati `wc -l < $file`

#stampo i file trovati nei direttori
for u in `cat $file`
do
echo DIRETTORIO $u:
    for z in `ls $u`
    do
        echo FILE $u/$z
        echo LINEA $X esima: `head -$X $u/$z | tail -1`
    done
done
rm $file #rimuovo il file temporaneo