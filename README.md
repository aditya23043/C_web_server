# HTTP Server in C

## Man Pages Utilized

<table>
    <tr>
        <td>socket</td>
        <td>htons</td>
        <td>recv</td>
        <td>sendfile</td>
    </tr>
    <tr>
        <td>bind</td>
        <td>listen</td>
        <td>strchr</td>
        <td>close</td>
    </tr>
    <tr>
        <td>sockaddr_in</td>
        <td>accept</td>
        <td>open</td>
        <td>fork</td>
    </tr>
</table>

## Build an executable
- You will need..
    - C compiler (any one of the following)
        - cc
        - gcc
        - clang
    - Build System (optional)
        - make

```
$ <cc/gcc/clang> --std=c99 --Wall -Werror src/main.c -o bin/server
```

- To run the server
```
$ ./bin/server <port>
```

## TODO
- [X] Display random text/html on the web server
- [X] Display the contents of the files passed as URL parameter
- [X] Handle concurrent clients using multi threading (fork)
- [ ] Implement Routing in the refactored code
- [ ] Make the backend process /POST requests as well

## Inspiration
- [Nir Lichtman](https://www.youtube.com/@nirlichtman)
- [Dr. Jonas Birch](https://www.youtube.com/@dr-Jonas-Birch)
