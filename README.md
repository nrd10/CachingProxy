# CachingProxy

This project code is built to form a caching proxy. The code is developed entirely in C++. The project has been configured and built in Docker.After building using sudo docker compose up on a Linux machine, a user can configure their favorite Internet browser to point at the server running this project. This proxy will then forward requests to and from the website they are trying to reach and their internet browser.

# Functionality

This section outlines each functionality the caching proxy contains.

# 1. Caching
The proxy caches HTTP GET requests to optimize the speed and performance of request delivery. 

# 2. Multithreading
The proxy is multithreaded and thus can handle multiple concurrent requests effectively.

# 3. Logging
The proxy produces a log in /var/log/erss/proxy.log which contains specific information about each request.

# 4. Request Types
This proxy is able to handle GET, POST and CONNECT HTTP requests.
