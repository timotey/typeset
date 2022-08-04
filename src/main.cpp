#include <iostream>
#include <type_traits>
#include <typeinfo>
#include"meta/set.hpp"
#include<memory>

size_t counter(){
    static size_t id = 0;
    return id++;
}

// a class to track the craation and destruction of values
template<class T>
struct logger{
    const size_t id;
    ~logger(){
        std::cerr << "Logger " << id << " Destroyed!\n";
    }
    logger():id(counter()){
        std::cerr << "Logger " << id << " Created!\n";
    }
    logger(logger&& lg):id(counter()){
        std::cerr << "Logger " << id << " is moved out of logger " << lg.id << "!\n";
    }
    logger(logger const& lg):id(counter()){
        std::cerr << "Logger " << id << " is copied from  logger " << lg.id << "!\n";
    }
    logger& operator=(logger&& lg){
        std::cerr << "Logger " << id << " is moved out of logger " << lg.id << " using assignment operator!\n";
        return *this;
    }
    logger& operator=(logger const& lg){
        std::cerr << "Logger " << id << " is copied from  logger " << lg.id << " using assgnment operator!\n";
        return *this;
    }
};

std::string demangle(const char* name);

int main() {
    // create a couple of typelevel sets to experiment with
    auto a = set(logger<int>{}, logger<double>{}, logger<float>{});
    auto b = set(logger<int>{}, logger<char>{});

    // do an operation with loggers to demonstrate that set operations are
    // efficient, and don't do more work than they have to
    std::cerr << "c\n";
    auto c = a&b;
    std::cerr << "c\n";

    //demonstarte that all operations produce correct results
    std::cerr
        << "  a  : "<< demangle(typeid(a).name()) << '\n'
        << "  b  : "<< demangle(typeid(b).name()) << '\n'
        << "a | b: "<< demangle(typeid(a|b).name()) << '\n' //set union
        << "a & b: "<< demangle(typeid(a&b).name()) << '\n' //set intersection
        << "a / b: "<< demangle(typeid(a/b).name()) << '\n';//set difference
}
