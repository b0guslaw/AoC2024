#pragma once

#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cstdint>

namespace Input
{
    template<typename T>
    std::vector<T> GetData(const std::string& path, char delim = ' ') {
        std::vector<T> data;
        try {
            std::ifstream infile(path);
            std::string line;
            while(std::getline(infile, line, delim)) {
                T element;
                std::stringstream input(line);
                while(input >> element)
                    data.push_back(element);
            }
        } catch (...) {
            std::cout << "An error occured!\n";
        }
        return data;
    }

    std::vector<std::vector<int>> GetGrid(const std::string& path) {
        std::vector<std::vector<int>> data;
        try {
            std::ifstream infile(path);
            std::string line;
            while(std::getline(infile, line)) {
                std::vector<int> temp;
                for (size_t j{0}; j < line.length(); j++) {
                    if (line[j] != ' ')
                        temp.push_back(line[j] - '0');
                }
                data.push_back(temp);
            }
        } catch (...) {
            std::cout << "An error occured!\n";
        }
        return data;
    }

    std::vector<std::string> GetEmptyNewLineData(const std::string& path) {
        std::ifstream infile(path);
        std::string temp;
        std::vector<std::string> data;
        for (std::string line; std::getline(infile, line); ) {
            if (line.empty()) {
                data.push_back(temp);
                temp.clear();
            }
            if (infile.eof()) {
                temp += line;
                data.push_back(temp);
            }
            temp += line;
            temp += " ";
        }
        return data;
    }

    std::vector<std::string> GetStringData(const std::string& path) {
        std::vector<std::string> data;
        try {
            std::ifstream infile(path);
            for (std::string line; std::getline(infile, line);) {
                data.push_back(line);
            }
        } catch (std::exception &e) {
            std::cout << e.what() << "\n";
        }
        return data;
    }

    std::string GetString(const std::string& path){
        std::ifstream ifs(path);
        std::string data((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
        return data;
    }

    std::vector<std::int64_t> GetIntData(const std::string& path){
        std::vector<std::int64_t> data;
        try {
            std::ifstream infile(path);
            for(std::string line; std::getline(infile, line); ) {
                if (line.empty()) {
                    data.push_back(0);
                } else {
                    data.push_back(std::stoll(line));
                }
            }
        } catch (std::exception& e) {
            std::cout << e.what() << "\n";
        }
        return data;
    }

    template<typename T>
    std::pair<std::vector<T>, std::vector<T>> Get2ColumnData(const std::string& path){
        std::vector<T> column1;
        std::vector<T> column2;
        try {
            std::ifstream infile(path);
            std::string line;
            while (std::getline(infile, line)) {
                std::istringstream lineStream(line);
                T value1, value2;

                if (!(lineStream >> value1 >> value2)) {
                    throw std::runtime_error("Invalid file format on line: " + line);
                }

                column1.push_back(value1);
                column2.push_back(value2);
            }
        } catch (std::exception& e) {
            std::cout << e.what() << "\n";
        }
        return std::make_pair(column1, column2);
    }
}