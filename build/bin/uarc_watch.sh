#!/bin/bash
while [ 1 ]
do
procID=`pgrep uarc_colld`
if [ "" == "$procID" ];
then
/home/uarc/bin/uarc_colld --daemon
fi
sleep 600
done &