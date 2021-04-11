#/bin/sh

#controllo di passare almeno due parametri
if test $# -lt 2
then 
	echo immettere almeno due parametri
	exit 1
fi


#controllo che il primo sia stret positivo
case $1 in  
*[!0-9]*) echo $1 non numerico positivo
	exit 4;;
*)	if test $1 -eq 0
	then echo non positivo
	fi
;;
esac



is_first=0
for i in $*
do
	if test $is_first != 0 #il controllo avverrà su tutti i parametri eccetto il primo (che controllo con il flag is_first
	then
	#testo che ci siano solo ger ass
		#controllo che il primo sia un nome assoluto
		case $i in
			/*);;
			*)	 echo $i deve essere un nome assoluto
    			exit 2
		;; 
		esac


	#controllo che il primo sia una directory navigabile
		if test ! -d $i && -r $i 
		then
		echo $i deve essere una directory 
		exit 3

		fi
	fi
	is_first=1
done

#salvo in path la directory
PATH=`pwd`:$PATH
export PATH

#creo un file temporaneo
file=/tmp/$$.txt
>$file

#Passo al file comandi ricorsivo rispettivamente: la gerarchia su cui operare, il numero di righe che deve avere ciascun file e il nome del file temporaneo
is_first=0
for k in $*
do

	if test $is_first != 0
	then

	esame170415_rec.sh $1 $k $file

	fi
	is_first=1
done

#stampo a video il file temporaneo per poi rimuoverlo.



echo FILE TROVATI: `wc -l < $file`

for i in `cat $file`
do
echo $i

echo cerca la linea n minore di $1:
read linea

echo `head -$linea $i | tail -1`

done
rm $file

