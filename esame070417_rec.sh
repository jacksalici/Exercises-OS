#!/bin/sh

#esame070417_rec.sh $F $i $file 

cd $2

#salvo i primi due parametri in variabili temporanee  
F=$1



    for i in *
    do 
        if 
            test -f $i -a -r $i
        then
               case $i in
               $F)
               sort -f $F > sorted
               echo `pwd`/sorted >> $3
               ;;
               *);;
               esac 
        fi
    done


for i in *
do 
    
    if 
        test -d $i -a -x $i
    then
       esame070417_rec.sh $F `pwd`/$i $3 
    fi
done


