#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <string>

#include "cgroupsv2.hpp"
struct clone_args {
    std::string process_max;
    std::string memory_max;
    std::string command;
    std::string rootfs;
    long stack_size;
    bool use_shell;
    bool disable_network;
    bool no_cgroup;
};


char* stack_memory(long stacksize){
    auto *stack = new (std::nothrow) char[stacksize];
    
    if (stack == nullptr){
        std::cout << "ERROR: cannot allocate stack memory\n";
        exit(EXIT_FAILURE);
    }
    return stack+stacksize;
}
template<typename Function>
void clone_process(Function&& f, int flags, long memsize, void *args){
    auto pid = clone(f, stack_memory(memsize), flags, args);
    wait(nullptr);
}

int run(char *name, bool use_shell){
    if (use_shell){
        system(name);
    } else {
        //char *_args[] = {(char *)name, (char *)0};
        //char *_args[] = {(char *)name, nullptr;
        //execvp(name, _args);
        if (name == "/bin/sh"){
            char *_args[] = {(char *)name, nullptr};
            execvp("/bin/sh", _args);
        } else {
            char *_args[] = {(char *)"/bin/sh",  (char *)"-c", (char *)name, nullptr};
            execvp("/bin/sh", _args);
        }
    }
    return 0;
}
void setuproot(char *folder){
    chroot(folder);
    chdir("/");
}
void setupenv(){
    clearenv();
    setenv("TERM", "xterm-256color", 0);
    setenv("PATH", "/bin/:/sbin/:/usr/bin:/usr/sbin", 0);
}
void limit_process_creation(void *arg){
    struct clone_args *_args = (struct clone_args*)arg;
    cgv2::utils::set_subgroup("container");

    cgv2::pids::attach_to_process(getpid());
    cgv2::pids::set_max(_args->process_max);
    //cgv2::memory::attach_to_process(getpid());
    //cgv2::memory::set_max(_args->memory_max);
}
int run_process_in_container(void *arg){
    struct clone_args *_args = (struct clone_args*)arg;
    std::string command = _args->command;
    run(command.data(), _args->use_shell);
    return 0;
}
int jail(void *arg){
    struct clone_args *_args = (struct clone_args*)arg;
    if (!_args->no_cgroup){
        limit_process_creation(arg);
    }

    setupenv();
    setuproot(_args->rootfs.data());

    mount("none", "/proc", "proc", 0, 0);
    mount("none", "/dev", "devtmpfs", 0, 0);
    int flags = SIGCHLD;
    if (_args->disable_network){
        //unshare(CLONE_NEWNET); // similar to putting a flag below
        flags = flags | CLONE_NEWNET;
    }

    clone_process(run_process_in_container, flags, _args->stack_size, arg);
    umount("/proc");
    umount("/dev");
    return 0;
}

void cli_entrypoint(void *arg){  
    clone_process(jail, CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD, 65536, arg);
}