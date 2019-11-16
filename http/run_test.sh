#!/bin/bash

port=8888
host=localhost
dir=my_website/

./server -p $port $dir &
server_pid=$!
./client -p $port -o tmp.html http://${host}/  
diff tmp.html $dir/index.html



mkdir tmp_dir/
./client -p $port -d tmp_dir/ http://${host}/  
diff $dir/index.html tmp_dir/index.html
rm -rf tmp_dir/

wget http://pan.vmars.tuwien.ac.at/osue/index.html
./client http://pan.vmars.tuwien.ac.at/osue/index.html > test.html
diff index.html test.html
wget http://pan.vmars.tuwien.ac.at/osue/countdown.js
mv countdown.js wget_countdown.js
./client -d . http://pan.vmars.tuwien.ac.at/osue/countdown.js
diff countdown.js wget_countdown.js

rm *.js 







kill $server_pid || echo "error"

