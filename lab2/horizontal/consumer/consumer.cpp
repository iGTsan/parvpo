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
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/NetException.h>
#include <Poco/JSON/Object.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/URI.h>

std::vector<int> matrix1_demo;
std::vector<int> matrix2_demo;
int got_data = 0;
int got_msg = 0;

template<typename T>
std::list<T> vectorToList(const std::vector<T>& vec) {
    return std::list<T>(vec.begin(), vec.end());
}

template<typename T>
std::vector<T> listToVector(const std::list<T>& vec) {
    return std::vector<T>(vec.begin(), vec.end());
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

// n*m * m*n
std::list<int> matrixMultiply(const std::list<int>& matrix1, const std::list<int>& matrix2, int n, int m, int c) {
    std::list<int> result(n * m, 0);

    auto it = result.begin();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            auto it1 = matrix1.begin();
            std::advance(it1, i * c);
            
            auto it2 = matrix2.begin();
            std::advance(it2, j);
            
            for (int k = 0; k < c; ++k) {
                *it += *it1 * *it2;
                // ::std::cout << *it1 << '*' << *it2 << " + ";
                std::advance(it1, 1);
                std::advance(it2, m);
            }
            // ::std::cout << *it << std::endl;
            ++it;
        }
    }
    
    return result;
}
bool isPerfectSquare(int num) {
    int squareRoot = std::sqrt(num);
    return squareRoot * squareRoot == num;
}

int handleProducerMessage(int num, std::vector<int> buf){
    if (num == -1){
        return 1;
    }
    else{
        buf.push_back(num);
        return 0;
    }
}

int* vectorToArray(const std::vector<int>& vec) {
    int* arr = new int[vec.size()]; // Create a new array with the same size as the vector
    
    // Copy the elements from the vector to the array
    for (size_t i = 0; i < vec.size(); i++) {
        arr[i] = vec[i];
    }
    
    return arr; // Return the pointer to the array
}

std::vector<int> getData(const std::string& requestContent) {
    std::string buffer;

    //std::size_t closingCurlyBracePos = requestContent.find("{");

    //std::cout << "closingCurlyBracePos: " << closingCurlyBracePos  << std::endl;

    if (1) {

        for (int i = 0; i < requestContent.length(); i++) {
            buffer += requestContent[i];
        }

        //buffer = requestContent.substr(0, closingCurlyBracePos + 1);
        //std::cout << buffer << " " << buffer.length() <<  std::endl;
    }
    else {
        return std::vector<int>{-1, -520, 0, 0};
    }

    nlohmann::json json = nlohmann::json::parse(buffer);

    int messageContent = json["message_content"];
    int messageRow = json["message_row"];
    int messageColoumn = json["message_coloumn"];

    //std::cout << messageType << " " << messageContent << std::endl;

    return std::vector<int>{0, messageContent, messageRow, messageColoumn};
}

void vectorInsert(std::vector<int>& vec, int num, int x, int y, int n, int m, int part_len) {
    int ind;
    if (m > n) {
        ind = (x%part_len) * m + y;
        if (x >= 3 * part_len) {
            ind += part_len * m;
        }
    } else {
        ind = (x) * m + y%part_len;
        if (y >= 3 * part_len)
            ind += part_len;
    }
    // std::cout << "Instert: " << x << ' ' << y << ' ' << n << ' ' << m << ' ' << ind << ' ' << num << std::endl;
    if (vec.size() < n * m) {
        vec.resize(n * m);
    }
    vec[ind] = num;
}

std::vector<int> getSize(int Msize, int NConsumers, int consumerNumber) {
    int NRows = ::std::sqrt(NConsumers);
    int n1 = Msize / NRows;
    if (consumerNumber % NRows == 0) {
        n1 += Msize % NRows;
    }
    int m1 = Msize;
    int m2 = Msize / NRows;
    if ((consumerNumber - 1) / NRows >= 2) {
        m2 += Msize % NRows;
    }
    int n2 = Msize;
    return std::vector<int>{m2, n2, m1, n1, Msize / NRows};
}


void sendMatrix(const std::list<int>& matrix, int consumerNumber, int n, int m) {
    Poco::URI uri("http://mega_consumer:8080/res");
    std::string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";

    try {
        // Prepare session
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

        // Create the request
        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
        req.setContentType("application/json");

        // Prepare JSON payload
        Poco::JSON::Object::Ptr jsonPayload = new Poco::JSON::Object;
        jsonPayload->set("consumerNumber", consumerNumber);
        jsonPayload->set("n", n);
        jsonPayload->set("m", m);

        Poco::JSON::Array matrixArray;
        for (const int& e : matrix) {
            // ::std::cout << e << " ";
            matrixArray.add(e);
        }
        // std::cout << n << ' ' << m << ' ' << matrix.size() <<  std::endl;
        jsonPayload->set("matrix", matrixArray);

        std::stringstream ss;
        jsonPayload->stringify(ss);

        // std::cout << ss.str() << std::endl;  

        // Set Content-Length
        req.setContentLength(ss.str().size());

        // Send the request
        std::ostream& os = session.sendRequest(req);
        os << ss.str();

        // Receive the response
        Poco::Net::HTTPResponse res;
        std::istream& is = session.receiveResponse(res);
        std::string responseString;
        if (res.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
            // std::getline(is, responseString);
            // std::cout << "Response from server: " << responseString << std::endl;
        } else {
            std::cout << "Error while sending request: " << res.getStatus() << " " << res.getReason() << std::endl;
        }
    } catch (const Poco::Net::NetException& e) {
        std::cerr << "Network exception: " << e.displayText() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main(int argc, char** argv) {
    crow::SimpleApp app;
    crow::logger::setLogLevel(crow::LogLevel::Warning);

    // std::cout << argc << ' ' << argv[1] << ' ' << argv[2] << ' ' << argv[3] << std::endl;
    const int consumerNumber = std::stoi(argv[1]);
    const int NConsumers = std::stoi(argv[2]);
    const int Msize = std::stoi(argv[3]);

    // std::cout << consumerNumber << ' ' << NConsumers << ' ' << Msize << std::endl;

    auto tmp_size = getSize(Msize, NConsumers, consumerNumber);
    int n1 = tmp_size[0];
    int m1 = tmp_size[1];
    int n2 = tmp_size[2];
    int m2 = tmp_size[3];
    int part_len = tmp_size[4];

    std::cout << consumerNumber << ' ' << NConsumers << ' ' << Msize << ' ' << n1 << ' ' << m1 << ' ' << n2 << ' ' << m2 << std::endl;

    auto timer1 = std::chrono::high_resolution_clock::now();

    CROW_ROUTE(app, "/")
    .methods("POST"_method)(
        [&](const crow::request& req, crow::response& res) {
            std::string requestContent = req.body;
            auto paramters = req.url_params;

            // std::cout << paramters << std::endl;
            
            auto tmp = getData(requestContent);
            int prod_type = std::stoi(paramters.get("pdtype")), prod_data = tmp[1];
            int x = tmp[2], y = tmp[3];

            // std::cout << prod_type << prod_data;

            if (prod_type == 1) {
                if (prod_data == -1) {
                    got_data += 1;
                }
                else {
                    vectorInsert(matrix1_demo, prod_data, x, y, n1, m1, part_len);
                    matrix1_demo.push_back(prod_data);
                }
            } 
            else if (prod_type == 2) {
                if (prod_data == -1) {
                    got_data += 1;
                }
                else {
                    vectorInsert(matrix2_demo, prod_data, x, y, n2, m2, part_len);
                }
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

            // std::cout << "End gotted" << std::endl;            
 
            if (got_data == 2) {
                // std::cout << "Start multiplying matrix" << std::endl;
                auto timer1 = std::chrono::high_resolution_clock::now();

                // printMatrix(vectorToList(matrix1_demo), n1, m1);
                // printMatrix(vectorToList(matrix2_demo), n2, m2);

                if (got_msg == n1*m1 + n2*m2) {
                    std::list<int> result = matrixMultiply(vectorToList(matrix1_demo), vectorToList(matrix2_demo), n1, m2, m1);
                    sendMatrix(result, consumerNumber, n1, m2);
                    // printMatrix(result, n1, m2);
                } else {
                    std::cout << "Not enough messages! Got: " << got_msg << " messages but need: " << n1*m1 + n2*m2 << std::endl;
                }

                auto timer2 = std::chrono::high_resolution_clock::now();   

                std::chrono::duration<double, std::milli> duration = timer2 - timer1;

                // std::cout << "Time: " << duration.count() << " ms" << std::endl;

                exit(0);
            }
        }
    );

    std::cout << "Starting server..." << std::endl;

    app.port(8080).multithreaded().run();

}
