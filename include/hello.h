#include<iostream>

class Hello{
    public:
        std::string sayHello(std::string name);
};


std::string Hello::sayHello(std::string name) {
    return "Hello " + name + ", this is Hello class!";
}