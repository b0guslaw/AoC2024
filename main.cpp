#include <iostream>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>

#include <ctre.hpp>
#include <ctre/range.hpp>

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

void day2() { // TODO add Part 1 back
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

std::uint64_t day3_1(const std::string& s) {
    using namespace ctre::literals;
    std::uint64_t result{0};
    auto start = s.cbegin();
    while (auto m = ctre::search<"mul\\(([0-9]+),([0-9]+)\\)">(start, s.cend())) {
        result += m.get<1>().to_number() * m.get<2>().to_number();
        start = m.get<0>().end();
    }
    return result;
}

std::uint64_t day3_2(const std::string& s) {
    bool enabled{true};
    std::uint64_t result{0};
    for (auto match : ctre::search_all<"do\\(\\)|don't\\(\\)|mul\\(([0-9]+),([0-9]+)\\)">(s)) {
        std::string_view view = match.get<0>();
        if (view.starts_with("mul")) {
            if (enabled) {
                result += match.get<1>().to_number() * match.get<2>().to_number();
            }
        }
        if (view.starts_with("don'")) {
            enabled = false;
        }
        if (view.starts_with("do(")) {
            enabled = true;
        }
    }
    return result;
}

std::uint64_t day4_1(const std::vector<std::string>& grid) {
    const auto rows = grid.size();
    const auto cols = grid[0].size();
    const std::string XMAS = "XMAS";

    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {0, -1},  // right, left
        {1, 0}, {-1, 0},  // up, down
        {1, 1}, {-1, -1}, // diagonal down-right, diagonal up-left
        {1, -1}, {-1, 1} // diagonal down-left, diagonal up-right
    };

    auto isXMASInDirection = [&](const std::size_t r, const std::size_t c, const int dr, const int dc) -> bool {
        for (std::size_t i = 0; i < XMAS.size(); ++i) {
            const auto next_r = r + i * dr;
            const auto next_c = c + i * dc;
            if (next_r >= rows || next_c >= cols || grid[next_r][next_c] != XMAS[i]) {
                return false;
            }
        }
        return true;
    };

    std::size_t count = 0;
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            if (grid[r][c] == XMAS[0]) {
                for (const auto& [dr, dc] : directions) {
                    count += isXMASInDirection(r, c, dr, dc);
                }
            }
        }
    }
    return count;
}

std::uint64_t day4_2(const std::vector<std::string>& grid) {
    const auto rows = grid.size();
    const auto cols = grid[0].size();

    const std::string MAS{"MAS"};
    const std::string SAM{"SAM"};
    auto isMASCross = [&](const std::size_t r, const std::size_t c) -> bool {
        const std::vector<std::pair<int, int>> diag1 = {{-1, -1}, {0, 0}, {1, 1}}; // Top-left to bottom-right
        const std::vector<std::pair<int, int>> diag2 = {{-1, 1}, {0, 0}, {1, -1}}; // Top-right to bottom-left

        auto matchesDiagonal = [&](const std::vector<std::pair<int, int>>& diagonal, const std::string& pattern) -> bool {
            for (std::size_t i = 0; i < pattern.size(); ++i) {
                const int nr = r + diagonal[i].first;
                const int nc = c + diagonal[i].second;
                if (nr < 0 || nr >= static_cast<int>(rows) || nc < 0 || nc >= static_cast<int>(cols) || grid[nr][nc] != pattern[i]) {
                    return false;
                }
            }
            return true;
        };

        return (matchesDiagonal(diag1, MAS) && matchesDiagonal(diag2, SAM)) ||
               (matchesDiagonal(diag1, MAS) && matchesDiagonal(diag2, MAS)) ||
               (matchesDiagonal(diag1, SAM) && matchesDiagonal(diag2, SAM)) ||
               (matchesDiagonal(diag1, SAM) && matchesDiagonal(diag2, MAS));
    };

    std::size_t count = 0;
    for (std::size_t r = 1; r < rows - 1; ++r) {
        for (std::size_t c = 1; c < cols - 1; ++c) {
            count += isMASCross(r,c);
        }
    }
    return count;
}

int main() {
    auto map = Input::GetStringData("infile.txt");
    std::cout << day4_2(map) << '\n';
};
