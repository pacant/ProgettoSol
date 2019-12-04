#!/bin/bash

echo "Supervisor e servers in esecuzione"
./supervisor 8 1> supervisor.log 2>stderror.log &
sleep 1

for (( i = 0; i < 10 ; ++i )); do
    ./client 5 8 20 1>> client.log &
    ./client 5 8 20 1>> client.log &
    sleep 1
done

for (( i = 0; i < 6; ++i )); do
    pkill -INT supervisor
    echo "SIGINT"
    sleep 10
done

echo "Launching double SIGINT"
pkill -INT supervisor
pkill -INT supervisor

sleep 1
./misura.sh >logmisure.log

echo "Terminato"