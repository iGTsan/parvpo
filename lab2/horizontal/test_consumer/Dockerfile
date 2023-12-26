# Use the official debian image as the base image
FROM debian:12

# Set the working directory inside the container
WORKDIR /app
# Install Boost dependencies
RUN apt-get update && \
    apt-get install -y build-essential && \
    apt-get install -y libboost-all-dev && \
    apt-get install -y libcurl4-openssl-dev

RUN apt-get install -y libpoco-dev

# Copy the consumer.cpp, crow_all.h, and json.hpp files to the working directory
COPY consumer.cpp .
COPY crow_all.h .
COPY json.hpp .


# Compile the consumer.cpp file
RUN g++ -o consumer consumer.cpp -lpthread -lcurl -I/usr/include/Poco -L/usr/lib -lPocoNet -lPocoUtil -lPocoJSON -lPocoXML -lPocoFoundation

CMD ["cd /app"]

# Set the entry point for the container
CMD ./consumer 1 $NConsumers $MSize
