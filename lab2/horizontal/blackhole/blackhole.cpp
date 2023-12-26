#include "crow_all.h"

int main(int argc, char** argv) {
    crow::SimpleApp app;
    crow::logger::setLogLevel(crow::LogLevel::Warning);
    CROW_ROUTE(app, "/")
    .methods("POST"_method)(
        [&](const crow::request& req, crow::response& res) {
            // std::cout << req.raw_url << std::endl;
            res.code = 200;
            res.set_header("Content-Type", "text/plain");

            res.end();
        }
    );

    std::cout << "Starting server..." << std::endl;

    app.port(8080).multithreaded().run();

}
