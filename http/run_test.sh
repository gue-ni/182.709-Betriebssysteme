#!/bin/bash

port=8888
host=localhost
dir=./my_website/

./server -p $port -i test.html $dir &
server_pid=$!
./client -p $port -o received.html http://${host}/  
diff received.html $dir/test.html
rm received.html
kill $server_pid || echo "error"

./client http://pan.vmars.tuwien.ac.at/osue/ > osue.html
diff osue.html testing/index.html
./client -d . http://pan.vmars.tuwien.ac.at/osue/countdown.js
diff countdown.js testing/countdown.js


./server -p $port ./my_website/
server_pid=$!
wget -p -nd http://${host}:${port}/ || echo "error with wget"

kill $server_pid || echo "error"

