#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

struct ResourceUsage {
    int threads = 0;
    long memory_kb = 0;

    static ResourceUsage from_pid(pid_t pid) {
        std::string path = "/proc/" + std::to_string(pid) + "/status";
        std::ifstream file(path);
        std::string line;
        ResourceUsage usage;

        while (std::getline(file, line)) {
            if (line.find("Threads:") == 0) {
                usage.threads = std::stoi(line.substr(8));
            } else if (line.find("VmRSS:") == 0) {
                std::istringstream iss(line.substr(6));
                iss >> usage.memory_kb;
            }
        }
        return usage;
    }
};

void spawn_clients(int num_clients, int port, std::vector<int>& sockets) {
    for (int i = 0; i < num_clients; ++i) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
            sockets.push_back(sock);
        } else {
            close(sock);
        }
    }
}

pid_t start_server(bool use_pool) {
    pid_t pid = fork();
    if (pid == 0) {
        if (use_pool) {
            execl("./server", "server", "--pool", nullptr);
        } else {
            execl("./server", "server", nullptr);
        }
        perror("execl failed");
        exit(1);
    }
    return pid;
}

void stop_server(pid_t pid) {
    kill(pid, SIGTERM);
    waitpid(pid, nullptr, 0);
}

void test_mode(const std::string& label, bool use_pool, int duration_sec, int num_clients) {
    std::cout << "=== [" << label << "] ===" << std::endl;
    pid_t pid = start_server(use_pool);
    std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for server init

    std::vector<int> client_sockets;
    spawn_clients(num_clients, 8080, client_sockets);

    for (int i = 0; i < duration_sec; ++i) {
        ResourceUsage usage = ResourceUsage::from_pid(pid);
        std::cout << "Sec " << i
                  << " | Threads: " << usage.threads
                  << " | Mem: " << usage.memory_kb << " KB" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (int sock : client_sockets) close(sock);
    stop_server(pid);
    std::cout << std::endl;
}

int main() {
    const int duration = 10;
    const int clients = 100;
    test_mode("THREAD POOL", true, duration, clients);
    std::cout << "==============\n" << std::endl;

    test_mode("NO POOL", false, duration, clients);
    return 0;
}
