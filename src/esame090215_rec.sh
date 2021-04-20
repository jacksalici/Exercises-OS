
#!/bin/sh
cd $1  #entro nella cartella passata come primo parametro
flag=1 #variabile che mi serve a capire se tutte le linee di ciascun file hanno almeno un carattere
stampager=1 #sul file, stampo una volta sola la directory corrente, poi si azzera


for i in * #per tutti i file/directory della cartella corrente...
do
	if test -f $i -a -r $i #...se è un file ed è leggibile...
		then 
			if test `wc -l < $i` = $2 #...se ha $2 linee...
			then
				for j in `cat $i` #... per ogni linea del file...
				do
				
					case $j in #...mi accerto che ci sia almeno un carattere, altrimenti flag si azzera
					*[0-9]*);;
					*) flag=0;;
					
					esac

				done

				if test $flag = 1 #Se tutti le righe avevano un numero, stampo il nome del file sul file passato a $3
				then 
					if test $stampager = 1 #solo una volta, stampo sul file anche la cartella corrente
					then
						stampager=0
						echo CARTELLA TROVATA: `pwd` > /dev/tty
					fi
					echo `pwd`/$i >> $3
					
				fi
			fi
		fi
	done

for k in * #eseguo la ricorsione in ogni cartella della dir corrente
do
	if test -r $k -a -d $k #accertandomi che sia una cartella e sia leggibile
	then
		esame090215_rec.sh `pwd`/$k $2 $3
	fi
done 


