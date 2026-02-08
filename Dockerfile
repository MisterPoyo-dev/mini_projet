FROM alpine:latest
WORKDIR /usr/local/app

COPY ./UDP/* /usr/local/app/UDP/
# installation paquets
RUN apk add gcc make

CMD["cd","/usr/local/app/UDP/","&&","make"]

