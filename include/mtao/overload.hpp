#pragma once


//Code to be used wwith std::visit
//https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
//std::variant<bool,int> x;
//std::visit(overloaded{
//[](bool x) {
//    std::cout << "bool" << std::endl;
//},
//[](int x) {
//    std::cout << "int" << std::endl;
//}
//}, x);


namespace mtao { 

template <class... Fs>
struct overloaded: Fs... { using Fs::operator()...; };

template <class... Fs>
overloaded(Fs...) -> overloaded<Fs...>;

}
