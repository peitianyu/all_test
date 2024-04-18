# 如果没有build文件夹, 新建一个
if [ ! -d "build" ]; then
    mkdir build
fi

cd build && cmake -DPYTHON_INCLUDE_DIR=/usr/include/python2.7 -DPYTHON_LIBRARY=/usr/lib .. && make -j6

./all_tests

cd ..