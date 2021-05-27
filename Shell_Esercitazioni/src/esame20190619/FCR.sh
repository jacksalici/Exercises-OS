
#!/bin/sh
#FCR.sh $H $i
#CAMBIO DIRECTORY

cd $2

counter=0
for i in * #controllo tutti gli elementi della directory corrente
do
	if test -f $i -a -r $i #mi assicuro che l'elemento corrente sia un file leggibile
	then
		if test `wc -l < $i` -lt $1
        then
            counter=`expr $counter + 1`
        fi
	fi	
done

if test $counter -ge 2
then
    echo DIR TROVATA `pwd`
fi

for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -r $k -a -d $k #accertandomi che sia una cartella e sia leggibile
	then
		FCR.sh $1 `pwd`/$k
	fi
done 