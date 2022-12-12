#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include "containd.hpp"
std::string get_opt(char ** begin, char ** end, const std::string & option){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        auto s = std::string(*itr);
        if ( s.front() == '"' ) {
            s.erase( 0, 1 ); // erase the first character
            s.erase( s.size() - 1 ); // erase the last character
        }

        return s;
    }
    throw std::runtime_error("Option doesn't exist"); 
}

bool opt_exists(char** begin, char** end, const std::string& option){
    return std::find(begin, end, option) != end;
}
/*
template<typename T>
void handle_argument(T *var, char *opt, T default_val, int argv[], int argc){
    if (!opt_exists(argv, argv+argc), opt){
        (*var) = default_val;
    } else {
        (*var) = get_opt(argv, argv+argc, opt);
    }
}*/

int main(int argc, char* argv[]){
    // options -p process limit
    // options -m set max memory
    // options -r set rootfs path
    // options -c set command

    struct clone_args args;
    
    // make a function
    if (!opt_exists(argv, argv+argc, "-p")){
        args.process_max = "max";
    } else {
        args.process_max = get_opt(argv, argv+argc, "-p");
    }
    if (!opt_exists(argv, argv+argc, "-m")){
        args.memory_max = "max";
    } else {
        args.memory_max = get_opt(argv, argv+argc, "-m");
    }
    if (!opt_exists(argv, argv+argc, "-r")){
        args.rootfs = "./rootfs";
    } else {
        args.rootfs = get_opt(argv, argv+argc, "-r");
    }
    if (!opt_exists(argv, argv+argc, "-c")){
        args.command = "/bin/sh";
    } else {
        args.command = get_opt(argv, argv+argc, "-c");
    }
    if (!opt_exists(argv, argv+argc, "-stack")){
        args.stack_size = 65536;
    } else {
        args.stack_size = std::stol(get_opt(argv, argv+argc, "-stack"));
    }
    
    // use a one liner here
    if (!opt_exists(argv, argv+argc, "--use-shell")){
        args.use_shell = false;
    } else {
        args.use_shell = true;
    }
    if (!opt_exists(argv, argv+argc, "--disable-network")){
        args.disable_network = false;
    } else {
        args.disable_network = true;
    }
    if (!opt_exists(argv, argv+argc, "--no-cgroup")){
        args.no_cgroup = false;
    } else {
        args.no_cgroup = true;
    }
    cli_entrypoint((void*)&args);
}