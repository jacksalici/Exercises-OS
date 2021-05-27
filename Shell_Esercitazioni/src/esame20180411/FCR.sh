#/bin/sh
#FCR.sh $primo $i $file


cd $2

LIST=
for i in *
do
LIST="$LIST $i"
done


for i in $LIST
do 
    if 
        test -f $i -a -r $i
    then
            if 
                test `wc -l < $i` -ge $1
            then 
                if 
                    test `wc -l < $i` -ge 5
                then 
                    r=`pwd`/$i.quinta
                    echo `tail -1 | head -5 $i` > $r
                    echo $r >> $3
                else
                    r=`pwd`/$i.NOquinta
                    >$r
                    echo $r >> $3
                fi
            fi    
    fi
done


for i in *
do 
    if 
        test -d $i -a -x $i
    then
        FCR.sh $1 `pwd`/$i $3
    fi
done