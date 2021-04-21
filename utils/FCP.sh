#/bin/sh
#CONTROLLO PARAMETRI

PATH=`pwd`:$PATH #aggiungo a $path la directory 
export PATH

file=/tmp/$$.txt #creo un file temporaneo
>$file

FCR.sh #PARAM #Passo al file comandi ricorsivo i parametri richiesti.

#STAMPO RISULTATI

rm $file #rimuovo il file temporaneo


PATH=`pwd`:$PATH #aggiungo a $path la directory 
export PATH


#organizzo in più fasi la chiamata del file ricorsivo
for i in $*
do
    FCR.sh $H $i #Passo al file comandi ricorsivo i parametri richiesti.
done

