#!/bin/sh
#CAMBIO DIRECTORY
cd $1

for i in * #controllo tutti gli elementi della directory corrente
do
	if test -f $i -a -r $i #mi assicuro che l'elemento corrente sia un file leggibile
	then
		#CONTROLLI
	fi	
done


for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -r $k -a -d $k #accertandomi che sia una cartella e sia leggibile
	then
		FCR.sh #PARAM
	fi
done 