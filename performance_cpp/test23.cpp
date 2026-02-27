// =============================================================================
// test23.cpp
// Linux 内核机制补充专题：PSI 压力观测与轻量告警（教学版）
// 编译：g++ -std=c++17 -O2 -pthread -o test23 test23.cpp
// =============================================================================

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#if !defined(_WIN32)
#include <unistd.h>
#endif

struct PsiSample {
    double some_avg10 = 0.0;
    double full_avg10 = 0.0;
    unsigned long long some_total = 0;
    unsigned long long full_total = 0;
};

static bool parse_avg10_line(const std::string& line, double& avg10, unsigned long long& total) {
    // line format: "some avg10=0.00 avg60=0.00 avg300=0.00 total=12345"
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        if (token.rfind("avg10=", 0) == 0) {
            avg10 = std::stod(token.substr(6));
        } else if (token.rfind("total=", 0) == 0) {
            total = std::stoull(token.substr(6));
        }
    }
    return true;
}

static bool read_psi(const std::string& path, PsiSample& out) {
    std::ifstream ifs(path);
    if (!ifs) return false;

    std::string line;
    while (std::getline(ifs, line)) {
        if (line.rfind("some", 0) == 0) {
            parse_avg10_line(line, out.some_avg10, out.some_total);
        } else if (line.rfind("full", 0) == 0) {
            parse_avg10_line(line, out.full_avg10, out.full_total);
        }
    }
    return true;
}

static std::string first_line(const std::string& path) {
    std::ifstream ifs(path);
    std::string line;
    if (std::getline(ifs, line)) return line;
    return "";
}

int main() {
#if defined(_WIN32)
    std::cout << "test23 仅支持 Linux（需要 /proc/pressure/*）\n";
    return 0;
#else
    std::cout << "PSI 轻量观测 (cpu/memory/io), 采样间隔 1s, 共 10 次\n\n";

    const std::vector<std::pair<std::string, std::string>> targets = {
        {"CPU", "/proc/pressure/cpu"},
        {"MEM", "/proc/pressure/memory"},
        {"IO ", "/proc/pressure/io"},
    };

    for (int i = 0; i < 10; ++i) {
        std::cout << "[sample " << std::setw(2) << (i + 1) << "]\n";
        for (const auto& t : targets) {
            PsiSample s;
            if (!read_psi(t.second, s)) {
                std::cout << "  " << t.first << "  (unavailable)\n";
                continue;
            }

            std::cout << "  " << t.first
                      << "  some.avg10=" << std::fixed << std::setprecision(2) << s.some_avg10
                      << "%  full.avg10=" << s.full_avg10 << "%\n";

            if (s.some_avg10 >= 10.0 || s.full_avg10 >= 1.0) {
                std::cout << "    WARN: pressure is high, consider throttling or shedding load\n";
            }
        }

        if (i == 0) {
            std::string load = first_line("/proc/loadavg");
            if (!load.empty()) std::cout << "  loadavg: " << load << "\n";
        }

        std::cout << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Done.\n";
    return 0;
#endif
}
