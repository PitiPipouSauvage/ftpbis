## How to use ?
To start the server just run the compiled version of the server.
To send files or folders to the server :
```bash
$ ./client file.txt 127.0.0.1
$ ./client myFolder/* 127.0.0.1
$ ./client myFolder 127.0.0.1
```
It sends folders by doing a recursive search through them.
