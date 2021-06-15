#!/bin/sh
#FCR.sh $abs $string $file commento per ricordarmi i vari parametri
#CAMBIO DIRECTORY
cd $1


flag=0 #var per verificare che ciu sia almeno un file 
for elem in * #controllo tutti gli elementi della directory corrente
do
	if test -f $elem #mi assicuro che l'elemento corrente sia un file
	then
		case $elem in #controllo se l'estensione è .string
		*\.$2) flag=1; break;; #se lo è, flag=1
		*);; 
		esac
	fi	
done

if test $flag = 1 #questo significa che c'è almeno un file che corrisponde ai parametri 
then
	echo `pwd` >> $3 #se flag = 1, inserisco il nome della directory nel file globale
fi



for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -d $k -a -x $k #accertandomi che sia una cartella e sia traversabile
	then
		FCR.sh `pwd`/$k $2 $3 #eseguo la ricorsione nelle sotto directory
	fi
done 