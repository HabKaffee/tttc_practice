# InstructionCounterPass

Проход, который считает число инструкций во всех блоках в IR.

## Сборка

```sh
cmake -S src -B build
cmake --build build --config Release --parallel 4
```

## Пример использования

### Дампим ll

```sh
clang -emit-llvm -S tests/test1/test.cpp -o tests/test1/test.ll   
```

### Проходим opt'ом с собраным пассом

```sh
opt -load-pass-plugin ./build/libCountInstructionsPass.so -passes=count-inst -disable-output tests/test1/test.ll 
```
