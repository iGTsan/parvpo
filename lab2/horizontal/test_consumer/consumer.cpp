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
#include <curl/curl.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/NetException.h>
#include <Poco/JSON/Object.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/URI.h>

std::vector<int> matrix1_demo;


template<typename T>
std::list<T> vectorToList(const std::vector<T>& vec) {
    return std::list<T>(vec.begin(), vec.end());
}
template<typename T>
std::vector<T> listToVector(const std::list<T>& vec) {
    return std::vector<T>(vec.begin(), vec.end());
}

void sendMatrix(const std::list<int>& matrix, int consumerNumber, int n, int m) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    std::string URL = "http://mega_consumer:8080/res";

    if(curl) {
        // Ссылка, на которую вы хотите отправить POST запрос
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        
        // Данные, которые будем отправлять в POST запросе
        // std::string jsonData = "{\"message\":\"Hello, World!\"}";

        // crow::json::wvalue x;
        // x["consumerNumber"] = consumerNumber;
        std::string jsonData = "{\"message\":\"Hello, World!\"}";

        // std::string jsonData = x.dump();

        // Указываем, что будем отправлять POST запрос
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        // Устанавливаем заголовки для JSON
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Выполнение запроса
        res = curl_easy_perform(curl);

        // Проверка на ошибки
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }

        // Освобождение ресурсов
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}


void sendMatrix2(const std::list<int>& matrix, int consumerNumber, int n, int m) {
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
            matrixArray.add(e);
        }
        jsonPayload->set("matrix", matrixArray);

        std::stringstream ss;
        jsonPayload->stringify(ss);

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
            std::getline(is, responseString);
            std::cout << "Response from server: " << responseString << std::endl;
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
    
    for (int i = 0; i < Msize*Msize; i++) {
        matrix1_demo.push_back(i);
    }

    sendMatrix2(vectorToList(matrix1_demo), consumerNumber, Msize, Msize);
}
