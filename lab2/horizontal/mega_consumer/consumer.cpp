#include <iostream>
#include "crow_all.h"
#include <vector>
#include "json.hpp"
#include <cmath>
#include <cstdlib>
#include <string>
#include <list>
#include <chrono>
#include <ctime>    
#include <atomic>

std::vector<int> matrix;
std::atomic<int> gotted_matrix(0);
std::mutex mtx;


template<typename T>
std::list<T> vectorToList(const std::vector<T>& vec) {
    return std::list<T>(vec.begin(), vec.end());
}

void printMatrix(const std::list<int>& matrix, int n, int m) {
    ::std::cout << "Matrix: " << n << ' ' << m << std::endl;
    auto it = matrix.begin();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            std::cout << *it << " ";
            it++;
        }
        std::cout << std::endl; 
    }
}

std::vector<int> getData(const std::string& requestContent) {
    std::string buffer;

    for (int i = 0; i < requestContent.length(); i++) {
        buffer += requestContent[i];
    }

    nlohmann::json json = nlohmann::json::parse(buffer);

    std::vector<int> res;

    res.push_back(json["consumerNumber"]);
    res.push_back(json["n"]);
    res.push_back(json["m"]);
    for (int i = 0; i < json["n"]; i++) {
        for (int j = 0; j < json["m"]; j++) {
            res.push_back(json["matrix"][i * res[2] + j]);
        }
    }

    return res;
}

void matrixInsert(std::vector<int>& matrix_to_insert, std::vector<int>& matrix, int Msize, int x, int y, int n, int m) {
    int offset = 3;
    {
        std::lock_guard<std::mutex> guard(mtx);
        // std::cout << "Instert: " << x << ' ' << y << ' ' << n << ' ' << m << ' ' << (x%n) * m + y << std::endl;
    }
    if (matrix_to_insert.size() < Msize * Msize) {
        matrix_to_insert.resize(Msize * Msize);
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            matrix_to_insert[(i + x)*Msize + j + y] = matrix[i*m + j + offset];
}

int main(int argc, char** argv) {
    crow::SimpleApp app;
    crow::logger::setLogLevel(crow::LogLevel::Warning);

    // std::cout << argc << ' ' << argv[1] << ' ' << argv[2] << ' ' << argv[3] << std::endl;
    const int NConsumers = std::stoi(argv[1]);
    const int Msize = std::stoi(argv[2]);

    auto timer1 = std::chrono::high_resolution_clock::now();

    CROW_ROUTE(app, "/res")
    .methods("POST"_method)(
        [&](const crow::request& req, crow::response& res) {
            std::string requestContent = req.body;
            auto paramters = req.url_params;

            // std::cout << requestContent << ' ' << req.raw_url << std::endl;
            
            auto tmp = getData(requestContent);
            int consumerNumber = tmp[0] - 1, n = tmp[1], m = tmp[2];
            int part_len = Msize / ::std::sqrt(NConsumers);
            int y = part_len * (consumerNumber % 3);
            int x = part_len * (consumerNumber / 3);
            
            matrixInsert(matrix, tmp, Msize, x, y, n, m);
            gotted_matrix.fetch_add(1, std::memory_order_relaxed);
            int gotted_matrix_ = gotted_matrix.load();

            // Construct an HTTP response
            res.code = 200;
            res.set_header("Content-Type", "text/plain");

            res.end();

            if (gotted_matrix_ == NConsumers) {
                printMatrix(vectorToList(matrix), Msize, Msize);

                auto timer2 = std::chrono::high_resolution_clock::now();   

                std::chrono::duration<double, std::milli> duration = timer2 - timer1;

                std::cout << "Time: " << duration.count() << " ms" << std::endl;

                exit(0);
            }

        }
    );

    std::cout << "Starting server..." << std::endl;

    app.port(8080).multithreaded().run();

}
