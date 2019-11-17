#!/bin/bash

port=$1
host=localhost
dir=./my_website/

echo "Your client requests files from server"
./server -p $port -i test.html $dir &
pid=$!
./client -p $port -o received.html http://${host}/  
diff received.html ${dir}test.html
rm received.html
kill $pid

echo "Your client requests files from server on the internet"
./client http://pan.vmars.tuwien.ac.at/osue/ > osue.html
diff osue.html testing/index.html

./client -d . http://pan.vmars.tuwien.ac.at/osue/countdown.js
diff countdown.js testing/countdown.js

echo "Your server processes requests from a web browser"
./server -p $port ./my_website/ &
pid=$!
wget -p -nd http://${host}:${port}/ 2>/dev/null
diff index.html ${dir}index.html
kill $pid

echo "incorrect requests"
./server -p $port ./my_website/ &
pid=$!
curl http://${host}:${port}/ -X POST
kill $pid

rm *.js *.html


