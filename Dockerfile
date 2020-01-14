FROM alpine:3.10

RUN apk add gcc musl-dev
RUN apk update
RUN apk add bash
RUN apk add make
