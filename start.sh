#!/bin/bash
#
### BEGIN INIT INFO
# Provides: CoAp
# Required-Start: $network
# Required-Stop: $network
# Default-Start: 2 3 5
# Default-Stop: 0 1 6
# Description: Service Skeletton
### END INIT INFO

user="root"

script=`readlink -n -m $0`

if [ "x$?" == "x1" ]; then
  script=`pwd -P | sed 's/[^\/]*\.sh$//' |sed 's/\/$//'`/start.sh
fi

name=`echo $script |rev | cut -d "/" -f 2 | rev`
dir=`echo $script |rev | cut -d "/" -f 2- | rev`
logfile=$dir/log/$name.log

# Find the java program running in /$dir/ folder and stores it's PID
is_running () {
  pid=`ps aux |grep -e [^\]]$dir |grep -v "/start.sh" |tr -s ' ' |cut -d ' ' -f 2 |tr '\n' ' '`
  if [[ $pid =~ ^[0-9\ ]+$ ]]; then
    PID=$pid
    return 1
  fi
  return 0
}
print_log(){
  tail -n 25 $logfile
}

for arg in "$@"
do
  case "$arg" in

  'start')
    start-stop-daemon --chuid $user --background --start --quiet --exec $dir/looprun.sh || return 2
    sleep 2
    ;;

  'stop')
    is_running
    if [ "x$?" == "x1" ]; then
      echo -e "Killing $name gracefully"
      kill -SIGTERM $PID
      sleep 60
      is_running
      if [ "x$?" == "x1" ]; then
        sleep 60
        is_running
        if [ "x$?" == "x1" ]; then
          echo -e "Killing $name gracefully again "
          kill -SIGTERM $PID
          sleep 60
          is_running
          if [ "x$?" == "x1" ]; then
            echo -e "sudo Killing $name"
            kill -KILL $PID
            sleep 1
          fi
        fi
      fi
      echo -e "Processes $PID Killed\n"
    else
      echo -e "$name wasn't running"
      RETVAL=1
    fi
    ;;

  'status')
    is_running
    if [ "x$?" == "x1" ]; then
      echo -e "$name is running:"
      print_log
    else
      echo -e "$name is STOPPED"
      RETVAL=1
    fi
    ;;

  'restart')
    echo -e "Restarting\n"
    /bin/bash $0 stop
    # sleep 1
    /bin/bash $0 start
    ;;

  *)
    echo "Usage: $0 { start | stop | status | restart }"
    RETVAL=1
    ;;
  esac
done

exit $RETVAL
