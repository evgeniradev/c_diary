FROM gcc:4.9
COPY . /usr/src/c_diary
WORKDIR /usr/src/c_diary
RUN gcc main.c c_diary.c -o main
CMD ["./main"]
