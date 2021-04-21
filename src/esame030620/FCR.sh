#!/bin/sh

#FCR.sh $C $i $file
cd $2

for i in *
do
    if test -d $i -a -x $i
    then
        case $i in
        $1?$1) echo `pwd`/$i >> $3
        ;;
        *) ;;
        esac
        echo ciao $i
        FCR.sh $1 $i $3
    fi
done
