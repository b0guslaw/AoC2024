#include <iostream>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <string>

#include "Input.hpp"

void day1() {
    auto [column1, column2] = Input::Get2ColumnData<std::uint64_t>("infile.txt");

    auto begin = std::chrono::steady_clock::now();
    std::sort(column1.begin(), column1.end());
    std::sort(column2.begin(), column2.end());

    std::uint64_t total{0}; // Part 1
    std::unordered_map<std::uint64_t, std::uint64_t> occurrence_map; // Part 2
    for (std::size_t i{0}; i < column1.size(); i++) {
        total += std::abs(static_cast<int64_t>(column1[i]) - static_cast<int64_t>(column2[i]));
        occurrence_map[column2[i]]++;
    }

    std::uint64_t total2{0};
    for (std::size_t i{0}; i < column1.size(); i++) {
        auto it = occurrence_map.find(column1[i]);
        if (it != occurrence_map.end()) {
            total2 +=  column1[i] * it->second;
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << total << ' ' << total2 << " took " << duration <<"\n";
}

bool safe_report(std::vector<int> report)
{
    auto asc = std::is_sorted(report.begin(), report.end());
    auto desc = std::is_sorted(report.rbegin(), report.rend());
    if (!asc && !desc)
    {
        return false;
    }
    bool safe{true};
    constexpr int MIN{1}, MAX{3};
    for (std::size_t i{0}; i < report.size() - 1; i++) {
        if (!safe) continue;
        auto curr = report[i];
        auto next = report[i + 1];
        if (std::abs(curr - next) < MIN || std::abs(next - curr) > MAX) {
            safe = false;
        }
    }
    return safe;
}

void day2() {
    auto reports = Input::GetStringData("infile.txt");
    std::uint64_t safe_reports{0};
    auto begin = std::chrono::steady_clock::now();
    for (const auto& string_report : reports) {
        std::istringstream is(string_report);
        std::vector<int> report;
        int number;
        while (is >> number) {
            report.push_back(number);
        }
        bool safe = safe_report(report);
        if (!safe)
        {
            for (std::size_t i{0}; i < report.size(); i++) {
                auto temp = report;
                temp.erase(temp.begin() + i);
                if (safe_report(temp)) {
                    safe = true;
                    break;
                }
            }
        }

        if (safe)
        {
            safe_reports++;
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << safe_reports << " after " << duration;
}

int main() {

}
