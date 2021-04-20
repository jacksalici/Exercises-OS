#!/bin/sh

#esame200121_rec.sh $file $G $*
cd $2

#salvo i primi due parametri in variabili temporanee e shifto 
file=$1
G=$2
shift
shift

cont=0 #per verificare che tutti i file siano parametri


    for i in *
    do 
        if 
            test -f $i
        then
               F=$i
               for j in $*
               do
                    if test $j = $F
                    then
                        cont=`expr $cont + 1`
                        break;
                    fi 
                done
                
        fi
    done

if test $cont -eq $#
then
    echo `pwd` >> $file
fi

for i in *
do 
    
    if 
        test -d $i -a -x $i
    then
        case $i in 
        *[0-9]*) esame200121_rec.sh $file `pwd`/$i $* ;;
        *);;
        esac
    fi
done




