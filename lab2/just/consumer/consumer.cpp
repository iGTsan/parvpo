#include <iostream>
#include "crow_all.h"
#include <vector>
#include "json.hpp"
#include <cmath>
#include <list>

std::vector<int> matrix1_demo;
std::vector<int> matrix2_demo;
int got_data = 0;

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


std::list<int> matrixMultiply(const std::list<int>& matrix1, const std::list<int>& matrix2, int size) {
    std::list<int> result(size * size, 0);

    auto it = result.begin();

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            auto it1 = matrix1.begin();
            std::advance(it1, i * size);
            
            auto it2 = matrix2.begin();
            std::advance(it2, j);
            
            for (int k = 0; k < size; ++k) {
                *it += *it1 * *it2;
                std::advance(it1, 1);
                std::advance(it2, size);
            }
            
            ++it;
        }
    }
    
    return result;
}
bool isPerfectSquare(int num) {
    int squareRoot = std::sqrt(num);
    return squareRoot * squareRoot == num;
}

std::vector<int> getData(const std::string& requestContent) {
    std::string buffer;

    for (int i = 0; i < requestContent.length(); i++) {
        buffer += requestContent[i];
    }

    nlohmann::json json = nlohmann::json::parse(buffer);

    int messageType = json["message_type"];
    int messageContent = json["message_content"];
    int messageRow = json["message_row"];
    int messageColoumn = json["message_coloumn"];

    return std::vector<int>{messageType, messageContent, messageRow, messageColoumn};
}


void vectorInsert(std::vector<int>& vec, int num, int x, int y, int n, int m) {
    if (vec.size() < n * m) {
        vec.resize(n * m);
    }
    vec[(x%n) * m + y] = num;
}

int main(int argc, char** argv) {
    crow::SimpleApp app;
    crow::logger::setLogLevel(crow::LogLevel::Warning);
    int Msize = std::stoi(argv[1]);

    auto timer1 = std::chrono::high_resolution_clock::now();

    CROW_ROUTE(app, "/")
    .methods("POST"_method)(
        [&](const crow::request& req, crow::response& res) {
            // Read the HTTP request
            std::string requestContent = req.body;

            //std::cout << requestContent << std::endl;
            
            // Check the type of producer
            auto tmp = getData(requestContent);
            int prod_type = tmp[0], prod_data = tmp[1];
            int x = tmp[2], y = tmp[3];

            //std::cout << prod_type << prod_data;

            if (prod_type == 1) {
                if (prod_data == -1) {
                    got_data += 1;
                }
                else {
                    vectorInsert(matrix1_demo, prod_data, x, y, Msize, Msize);
                    // matrix1_demo.push_back(prod_data);
                }
            } 
            else if (prod_type == 2) {
                if (prod_data == -1) {
                    got_data += 1;
                }
                else {
                    vectorInsert(matrix2_demo, prod_data, x, y, Msize, Msize);
                    // matrix2_demo.push_back(prod_data);
                }
            } 
            else {
                // Unknown producer type
                // ...
                ;
            }

            // Construct an HTTP response
            std::string responseContent = std::to_string(prod_data); // Placeholder for the response content
            res.code = 200;
            res.set_header("Content-Type", "text/plain");
            res.body = responseContent;

            res.end();
        }
    );

    CROW_ROUTE(app, "/end").methods("POST"_method)(
        [&](const crow::request& req, crow::response& res) {
            got_data += 1;

            if (got_data == 2) {

                if (isPerfectSquare(matrix1_demo.size()) && isPerfectSquare(matrix2_demo.size())) {
                    std::list<int> result = matrixMultiply(vectorToList(matrix1_demo), vectorToList(matrix2_demo), Msize);
                    printMatrix(result, Msize, Msize);
                }

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
