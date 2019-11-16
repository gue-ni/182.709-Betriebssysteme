#!/bin/bash

port=8888
host=127.0.0.1
dir=my_website/

./server -p $port $dir &
server_pid=$!
./client -p $port -o tmp.html http://${host}/  
diff tmp.html $dir/index.html

mkdir tmp_dir/
./client -p $port -d tmp_dir/ http://${host}/  
diff $dir/index.html tmp_dir/index.html
rm -rf tmp_dir/

kill $server_pid

