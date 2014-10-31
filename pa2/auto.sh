#!/bin/bash
rm -f pa.zip
vim run
zip pa.zip pnqueen run
./qsub pa.zip
