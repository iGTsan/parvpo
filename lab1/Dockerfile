FROM alpine
RUN apk update && apk add g++ git
RUN apk add make
WORKDIR /lab
CMD git clone https://github.com/iGTsan/parvpo.git; cd parvpo/lab1; make; ./run > /vol/res