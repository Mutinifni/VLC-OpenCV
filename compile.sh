echo "compiling $1"
g++ -ggdb `pkg-config --cflags opencv libvlc` -o `basename $1 .cpp` $1 `pkg-config --libs opencv libvlc`;
echo "Output file -> ${1%.*}"
