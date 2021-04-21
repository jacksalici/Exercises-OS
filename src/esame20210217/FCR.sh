#!/bin/sh

#esame170221_rec.sh $H $K $i $file 

cd $3
H=$1
K=$2
file=$4


flag=0

for i in *
do 
    if test -f $i -a -r $i
    then
            if test `wc -l < $i` -eq $K
            then
                flag=`expr $flag + 1`
            fi 
    fi
done

if test $flag -ge $H
then
    echo $3 >> $file
fi

for i in *
do 
 
    if 
        test -d $i -a -x $i
    then
      FCR.sh $H $K `pwd`/$i $file   
    fi
done
