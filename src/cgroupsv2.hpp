#include <stdexcept>
#include <fstream>
#include <string>


std::string subgroup_name;

namespace cgv2::utils {
    void set_subgroup(std::string n){
        subgroup_name = n;
    }
    template<typename T>
    void write_rule(std::string path, T value){
        std::ofstream fp;
        fp.open(path, std::ofstream::out | std::ofstream::app);
        if (!fp.is_open()){
            throw std::runtime_error(std::string("FP to ") + path + std::string(" failed"));
        }
        fp << value;
        fp.close();
    }
    template<typename T>
    void write_rule_interface(std::string interface, std::string subgroup, std::string name, T value){
        std::string path = std::string("/sys/fs/cgroup/") + interface + std::string("/") + subgroup + std::string("/") + name;
        write_rule(path, value);
    }
}
namespace cgv2::memory {
    void attach_to_process(int pid){
        cgv2::utils::write_rule_interface("memory", subgroup_name, "cgroup.procs", pid);
    }
    template<typename T>
    void set_max(T max){
        cgv2::utils::write_rule_interface("memory", subgroup_name, "memory.max", max);
    }
}
namespace cgv2::pids {
    void attach_to_process(int pid){
        cgv2::utils::write_rule_interface("pids", subgroup_name, "cgroup.procs", pid);
    }
    template<typename T>
    void set_max(T max){
        cgv2::utils::write_rule_interface("pids", subgroup_name, "pids.max", max);
    }
}