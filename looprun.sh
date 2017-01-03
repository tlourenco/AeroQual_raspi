#!/usr/bin/env bash

script=`readlink -n -m $0`

if [ "x$?" == "x1" ]; then
  script=`pwd -P | sed 's/[^\/]*\.sh$//' |sed 's/\/$//'`/start.sh
fi

#name=`echo $script |rev | cut -d "/" -f 2 | rev`
name=`echo $script |rev | cut -d "/" -f 2 | rev`
dir=`echo $script |rev | cut -d "/" -f 2- | rev`
logfile=$dir/log/$name.log

cd $dir

echo "$name started at `date '+%Y-%m-%d %H:%M:%S'`" > $logfile

while [ 1 ]
        do
            $dir/bin/AeroQual /dev/ttyUSB0 /dev/ttyUSB1 /dev/ttyUSB2 &>> $logfile 2>&1
            echo -e "\n$name crashed at `date '+%Y-%m-%d %H:%M:%S'`\nRestarting\n" >> $logfile
            sleep 15
        done
