#!/bin/sh
#FCR.sh $X $i $file 

#CAMBIO DIRECTORY
cd $2

flag=0
for i in * #controllo tutti gli elementi della directory corrente
do

	if test -f $i -a -r $i #mi assicuro che l'elemento corrente sia un file leggibile
	then
		if test `wc -l < $i` -le $1
		then
			flag=1
			break;
		fi

	fi
	if test -d $i
	then
	flag=1;
	break;
	fi
done

if test $flag -eq 0
then
	echo `pwd` >> $3
fi

for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -r $k -a -d $k #accertandomi che sia una cartella e sia leggibile
	then
		FCR.sh $1 `pwd`/$k $3 #PARAM
	fi
done 