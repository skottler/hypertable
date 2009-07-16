#!/bin/sh

if test ${INSTALL_DIR}
  then HT_HOME=${INSTALL_DIR}
else
  HT_HOME="$HOME/hypertable/current"
fi
SCRIPT_DIR=`dirname $0`
CONFIG=$SCRIPT_DIR/ScanLimit_test.cfg

restart_servers() {
  $HT_HOME/bin/clean-database.sh
  $HT_HOME/bin/start-all-servers.sh --no-thriftbroker local \
      --config $CONFIG
}

test() {
  restart_servers
  $HT_HOME/bin/hypertable --no-prompt < \
      $SCRIPT_DIR/ScanLimit_test.hql > ScanLimit_test.output

  diff ScanLimit_test.output $SCRIPT_DIR/ScanLimit_test.golden
  if [ $? != 0 ] ; then
     echo "Test failed, exiting ..."
     exit 1
  fi

  sleep 2
  n1=`ls -R1 $HT_HOME/fs/local/hypertable/tables/METADATA | wc -l`
  sleep 4
  n2=`ls -R1 $HT_HOME/fs/local/hypertable/tables/METADATA | wc -l`

  if [ $n1 -ne $n2 ] ; then
     echo "Test failed, exiting ..."
     exit 1
  fi

  echo "Test passed."
  exit 0
}

test