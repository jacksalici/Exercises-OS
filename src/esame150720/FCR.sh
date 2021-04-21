#!/bin/sh

#FCR.sh $L $i $file

#CAMBIO DIRECTORY
cd $2

for i in * #controllo tutti gli elementi della directory corrente
do
	if test -f $i -a -r $i #mi assicuro che l'elemento corrente sia un file leggibile
	then
		if test `wc -l < $i` -eq $1
        then
            car=`wc -c < $i`
            if test `expr $car / $1` -gt 10
            then
                echo `pwd`/$i >> $3
            fi
            
        fi 
	fi	
done


for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -r $k -a -d $k #accertandomi che sia una cartella e sia leggibile
	then
		FCR.sh $1 `pwd`/$k $3
	fi
done 