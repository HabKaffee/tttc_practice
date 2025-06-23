# Inline Pass
Embends functions calls without arguments and return values into the body of the calling function

## Setup 
```sh
export PATH=/PATH/TO/LLVM/BUILD/bin/:$PATH
```

## Build
```sh
cmake ..
make
```

## Use
```sh
clang -O0 -S -emit-llvm -Xclang -disable-O0-optnone test.cpp -o test.ll
opt -load-pass-plugin=/PATH/TO/PASS/BUILD/MyInlinePass.so -passes=my-inline-pass -S test.ll -o out.ll 
```