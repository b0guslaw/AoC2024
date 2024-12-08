#include <iostream>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>

#include <ctre.hpp>
#include <set>

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

    auto isMASCross = [&](const std::size_t r, const std::size_t c) -> bool {
        constexpr std::array<std::pair<int, int>, 3> DIAG1 = {{{-1, -1}, {0, 0}, {1, 1}}}; // Left, Center, Right
        constexpr std::array<std::pair<int, int>, 3> DIAG2 = {{{-1, 1}, {0, 0}, {1, -1}}}; // Right, Center, Left
        constexpr std::string_view MAS = "MAS";
        constexpr std::string_view SAM = "SAM";

        auto matchesDiagonal = [&](const std::array<std::pair<int, int>, 3>& diagonal, std::string_view pattern) -> bool
        {
            for (std::size_t i = 0; i < pattern.size(); ++i) {
                const int next_r = r + diagonal[i].first;
                const int next_c = c + diagonal[i].second;
                if (next_r < 0 || next_r >= static_cast<int>(rows) ||
                    next_c < 0 || next_c >= static_cast<int>(cols) ||
                    grid[next_r][next_c] != pattern[i])
                    {
                        return false;
                    }
            }
            return true;
        };

        return
            (matchesDiagonal(DIAG1, MAS) && matchesDiagonal(DIAG2, SAM)) ||
            (matchesDiagonal(DIAG1, MAS) && matchesDiagonal(DIAG2, MAS)) ||
            (matchesDiagonal(DIAG1, SAM) && matchesDiagonal(DIAG2, SAM)) ||
            (matchesDiagonal(DIAG1, SAM) && matchesDiagonal(DIAG2, MAS));
    };

    std::size_t count = 0;
    for (std::size_t r = 1; r < rows - 1; ++r) {
        for (std::size_t c = 1; c < cols - 1; ++c) {
            count += isMASCross(r,c);
        }
    }
    return count;
}

std::uint64_t day5_1(const std::vector<std::string>& pages) {
    auto begin = std::chrono::steady_clock::now();
    std::uint64_t total{0};

    // First, parse the rules
    std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> rules;
    std::size_t page_offset{0};
    for (const auto& page : pages) {
        if (page.empty()) {
            break; // Finished reading the rules
        }
        std::string_view sv(page);
        std::uint32_t page_number, page_rule;
        std::from_chars(sv.data(), sv.data() + 2, page_number);
        std::from_chars(sv.data() + 3, sv.data() + sv.size(), page_rule);
        if (rules.contains(page_number)) {
            rules[page_number].push_back(page_rule);
        } else {
            rules[page_number] = {page_rule};
        }
        ++page_offset;
    }

    auto parse_line = [](const std::string& input) -> std::vector<int> {
        std::vector<int> result;
        std::stringstream ss(input);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(std::stoi(token));
        }
        return result;
    };

    // Now, check the updates
    std::unordered_set<std::uint32_t> current_page;
    using std::operator""sv;
    for (std::size_t i = page_offset + 1; i < pages.size(); ++i) {
        bool legal{true};
        current_page.clear();
        auto split = parse_line(pages[i]);
        for (const auto& page : split) {
            if (!rules.contains(page)) {
                current_page.insert(page);
                continue; // Special case: Page has no ordering rules
            }
            const auto& preconditions = rules.at(page);
            for (auto& update : preconditions) {
                if (current_page.contains(update)) {
                    legal = false;
                    break;
                }
            }
            if (legal) { current_page.insert(page); }
        }
        if (legal) {
            total += split[split.size()/2];
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << total << ' ' << total << " took " << duration <<"\n";
    return total;
}

int main() {
    auto pages = Input::GetStringData("infile.txt");
    std::cout << day5_1(pages);
}
