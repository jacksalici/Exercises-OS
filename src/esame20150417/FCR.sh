
#!/bin/sh

cd $2 #entro nella cartella passata come primo parametro
line_f=0 #numero righe che terminano per f

for i in * #per tutti i file/directory della cartella corrente...
do
	if test -f $i -a -r $i #...se è un file ed è leggibile...
		then 
				for j in `cat $i` #... per ogni linea del file...
				do
				
					case $j in #...controllo se termina per f e nel caso aumeno il contatore
					*f)
						line_f=`expr $line_f + 1`;;
					*) ;;
					
					esac

				done
				if test $line_f = $1 #Se $1 righe terminano per f, stampo il nome del file sul file passato a $3 come nuova riga
				then 
					echo `pwd`/$i >> $3
					
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


