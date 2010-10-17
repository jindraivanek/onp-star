r=$1
shift
for i in `seq 1 $r`; do 
  if time $*; 
    then echo hotovo $i; 
    else break; 
  fi 
done
