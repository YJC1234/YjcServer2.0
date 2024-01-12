#include <BaseNet/task.h>
#include <iostream>

using namespace YServer;

task<int> test2() {
    std::cout << "3";
    co_return 4;
}

task<int> test1() {
    std::cout << "2";
    int a = co_await test2();
    std::cout << a;
    co_return 5;
}

task<> example() {
    std::cout << "1";
    int b = co_await test1();
    std::cout << b << std::endl;
}

int main() {
    example().resume();
}