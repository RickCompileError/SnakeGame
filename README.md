# Snake Game

## How to download
```bash=
git clone https://github.com/RickCompileError/SnakeGame.git
```

## Dependency
```bash=
sudo apt install make gcc
sudo apt-get install libncurses5-dev libncursesw5-dev
```

## Compile
```bash=
# First time need to "git checkout network-snake" first
make
```

## How to play on localhost
```bash=
# for server
make active_server
# for client
make active_client
```

## How to play on different sites
```bash=
# for server
./server [port] 2> servlog.txt
# for client
./client [server ip] [server port] 2> clilog.txt
```

## How to clean up
```bash=
make clean
```
