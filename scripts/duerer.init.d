#!/bin/bash
#
#   Duerer image server
#
#  Add to system with:
#    sudo update-rc.d duerer defaults 90

### BEGIN INIT INFO
# Provides: duerer
# Required-Start:
# Required-Stop:
# Should-Start:
# Should-Stop:
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Start and stop Duerer image server
# Description: Duerer image server
### END INIT INFO

NAME=duerer
EXEC=/usr/bin/$NAME
CONFIG=/etc/$NAME/config.xml
USER=$NAME
PID_FILE=/var/run/$NAME/$NAME.pid
LOG=/var/log/$NAME/$NAME.log

START_STOP_OPTS="-x $EXEC -n $NAME -c $USER -p $PID_FILE"


start() {
    mkdir -p $(dirname $PID_FILE)
    start-stop-daemon --start $START_STOP_OPTS -m -b -- --config $CONFIG \
      --log $LOG --log-rotate-dir $(dirname $LOG)
}


stop() {
    start-stop-daemon --stop $START_STOP_OPTS
}


status() {
    start-stop-daemon --status $START_STOP_OPTS
}


################################################################################
# Main switch statement
#
case "$1" in
    start) start ;;
    stop) stop ;;
    restart) stop; start ;;
    status) status ;;

    *)
      echo "Syntax: $0 <start | stop | restart | status>"
      exit 1
    ;;
esac
