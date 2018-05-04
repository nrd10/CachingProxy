# Caching Proxy Test Case

***To test the following test cases, you SHOULD open three terminals. You will run your caching proxy on one terminal by typing:***

 `sudo docker-compose up.` 

You will run your original server on the second terminal by typing:

`nc -l 8000`

You will run your client on the third terminal by typing:

`nc <hostname> 12345` 

***NOW YOU ARE READY TO TEST***

## 1. VERY SIMPLE

`client:`

```http
GET /myawesome_cache_proxy.txt HTTP/1.1
Host: <hostname>:8000


```



You should be able to see the same message on the terminal where the original server lives in.

`server:`

```http
HTTP/1.1 200 OK
Content-Length: 25

<html>Hello World!</html>
```



You should be able to see the response on the terminal where the client lives in.

 

## 2. POST

`client`:

```http
POST /change_my_awesome_proxy/ HTTP/1.1
Content-Length: 3

ONE
```

You should be able to see the same message on the server side

`server`:

```http
HTTP/1.1 200 OK
Content-Length: 9

DATA SAVE
```

You should be able to see the response on the terminal where the client lives in

## 3. CONNET

`client`:

```http
CONNECT <hostname>:8000 HTTP/1.1


```

Then on server, type:

```http
HTTP/1.1 200 OK

```

Then what you can enter characters on either side, you can see the exact same words appearing on the other side. If you type `Ctrl+D` on either side(client or original server), you SHALL see the other side also exit. The proxy will still be running.



### 4. MALFORMED

---

#### _(1)Bad Host Name_

`client`:

```http
GET /my_awesome_cache_proxy.txt HTTP/1.1
Host: 12345
```

`client`:

```http
HTTP/1.1 400 Bad Request
Content-Length: 11

Bad Request
```



#### _(2)Bad Protocol_

`client`:

```http
GET /my_awesome_cache_proxy.txt HTT39302
Host: <hostname>

```

`client`:

```http
HTTP/1.1 400 Bad Request
Content-Length: 11

Bad Request
```



#### _(3)Bad Token_

`client`:

```http
GET /my_awesome_cache_proxy.txt HTTP/1.1
Host: <hostname>:8000
MY_MALICIOUS_TOKEN

```

`client:`

```http
HTTP/1.1 400 Bad Request
Content-Length: 11

Bad Request
```



#### _(4)Post No Content Length_

`client`:

```http
POST /my_awesome_cache_proxy.txt HTTP/1.1
Host: <hostname>:8000

FJEWIO
```

`client:`

```http
HTTP/1.1 400 Bad Request
Content-Length: 11

Bad Request
```



#### _(5)Bad Response_

`client:`

```http
GET /my_awesome_cache_proxy.txt HTTP/1.1
Host: <hostname>:8000


```

`server:`

```http
HTTP/1.1 200 OK
MY_MALLICIOUS_TOKEN

//(OR)

HTTP/1.1 200 OK
ETag: "39138-483831"
Date: Fri, 2 Mar 2018 23:24:56 GMT

THIS RESPONSE HAS NO CONTENT LENGTH OR CHUNKED ENCODING, IT'S BROKEN!

//(OR)

```

`client:`

```http
HTTP/1.1 502 Bad Gateway

Bad Gateway
```



## 5. CACHE AND CONCURRENT

***THESE ARE NOT QUITE POSSIBLE TO USE NETCAT***

***PLEASE GO TO THE FOLLOWING WEBSITES TO TEST OUR CACHING PROXY:***

[NBA](www.nba.com)

[YouTube](www.youtube.com)

[ESPN][www.espn.com]

[GOOGLE](www.google.com)

...

You can use the log file to check our proxy's functionality on cache and concurrency





