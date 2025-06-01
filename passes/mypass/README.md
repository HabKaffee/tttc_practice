# InstructionCounterPass

Проход, который оборачивает циклы внутри функции в вызовы loop_start() и loop_end()

## Сборка Pass

```sh
cmake -S src -B build
cmake --build build --config Release --parallel 4
```

## Пример использования

### Собираем pass-plugin

```sh
cmake -S src -B build
cmake --build build --config Release --parallel 4
```

### Собираем хуки для функций

```sh
clang++ -c ./example/wrap_funcs.cpp -o ./example/wrap_funcs.o
```

### Дампим IR, делаем проход opt и линкуем в исполняемый файл

```sh
clang++ -O0 -emit-llvm -S example/example.cpp -o example/example.ll
opt -load-pass-plugin build/LoopWrapperPass.so -passes=loop-wrapper -S example/example.ll -o example/example_opt.ll 
clang++ example/example_opt.ll example/wrap_funcs.o -o example/example.a
```

### Проверяем работоспособность

```sh
./example/example.a
```
