
cmake ndserver/CMakeLists.txt \
    -B../build/ndserver

make -C../build/ndserver


cmake ndclient/CMakeLists.txt \
    -B../build/ndclient

make -C../build/ndclient
