
#!/bin/sh

#FCR.sh $D $Y $i $file

#CAMBIO DIRECTORY
cd $3
nfile=0
base=`basename $3`

if test $base = $1
then
    for i in * #controllo tutti gli elementi della directory corrente
    do
    	if test -f $i -a -r $i #mi assicuro che l'elemento corrente sia un file leggibile
    	then
    		if test `wc -c < $i` -ge $2 #se il file ha almeno Y lettere, aggiorno il numero file trovati
            then
            nfile=`expr $nfile + 1`
            echo `pwd`/$i >> $4
            fi
    	fi	
    done
fi

#stampo tutte le directory in cui trovo almeno 1 file
if test $nfile -ge 1
then
echo DIR ACCETTABILE: `pwd`
fi


for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -d $k #accertandomi che sia una cartella e sia leggibile
	then
		FCR.sh $1 $2 `pwd`/$k $4 #PARAM
	fi
done 
