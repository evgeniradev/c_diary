# CDiary

A C-based app that manages records using a binary file as storage.

## Installation

Please, use [Docker](https://docs.docker.com/) to install this app.

Run the below setup command to build the container.
```
$ docker build -t c_diary .
```

Start the app:
```
$ docker run -i -v c_diary:/usr/src/c_diary -t c_diary
```
