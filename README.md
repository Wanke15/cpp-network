# cpp-network
network service

# Build
```bash
g++ --std=c++11 main.cpp -o server && ./server
```

# Make request
```bash
curl "http://127.0.0.1:8080/hi?name=jeff"
### Hello jeff, this is Hello class!
```