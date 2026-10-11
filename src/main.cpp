#include "core/Application.h"
#include <cstdlib>
#include <exception>
#include <iostream>

int main(){
    try{
        Application app;
        app.run();
    } catch (const std::exception& e){
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}