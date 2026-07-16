#pragma once
#include <string>
#include <vector>
#include <map>

struct Module {
    std::string cell_type;
    float split_mass;
    std::map<std::string, int> childs;

    std::map<std::string, bool> flags;
    std::map<std::string, float> params;
};

struct Genome {
    std::string name;
    int startModule;
    std::vector<Module> modules;
};