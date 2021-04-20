#/bin/sh
#esame110418_rec.sh $i $ultimo $file


cd $1

for i in *
do 
    if 
        test -f $i -a -r $i -a -w $i
    then
            if 
               test $i = $2.txt
            then 
                 echo "`pwd`/$2.txt" >> $3
                 echo "`wc -c <$i`" >> $4
            fi    
    fi
done


for i in *
do 
    if 
        test -d $i -a -x $i
    then
        esame050419_rec.sh `pwd`/$i $2 $3 $4
    fi
done