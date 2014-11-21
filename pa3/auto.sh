#!/bin/bash
for M in `seq 0 32 64`
    for k in `seq 0 20 100`
        rm -f pa.zip
        echo -e '#!/bin/sh\n./my_lock 8 '"$M $k" > run
        zip pa.zip my_lock run
        ./qsub pa.zip
        sleep 1m
    done
done

