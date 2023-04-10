#include <iostream>
#include <type_traits>

class A {
public:
    int test(int x) { return x; }
};

template<class T>
typename std::enable_if<std::is_integral<T>::value, bool>::type is_odd(T i) {
    return bool(i % 2);
}

template<class T, typename =typename std::enable_if<!std::is_integral<T>::value, bool>::type>
bool is_odd(T i) {
    return false;
}

template<class T,
        class = typename std::enable_if<std::is_integral<T>::value>::type>
bool is_even(T i) {
    return !bool(i % 2);
}

//template<typename F, typename... Args>
//using return_type = typename std::result_of_t<F(Args...)>;

template<typename F, typename... Args>
using return_type = decltype(std::declval<F>()(std::declval<Args>()...));

void test_enable_if() {
    A a;
    std::cout << "i is odd: " << is_odd(a) << std::endl;
    //std::cout << "i is even: " << is_even(a) << std::endl;

}

//void test_return_type() {
//    A a;
//    auto f = [](int param) { return param; };
//    using T =return_type<decltype(f), int>(f, 2);
//}


int main() {
    test_enable_if();
    return 0;
}