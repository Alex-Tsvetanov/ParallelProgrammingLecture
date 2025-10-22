# 🐋 Docker Setup - Complete Guide

## 📦 What's Included in the Docker Image

The Docker image provides a **fully configured environment** with:

- ✅ **Ubuntu 22.04** base system
- ✅ **GCC 12** (full C++20 support including coroutines)
- ✅ **Clang 14** (alternative compiler)
- ✅ **CMake & Ninja** (modern build tools)
- ✅ **OneTBB library** (Intel Threading Building Blocks)
- ✅ **Development tools**: gdb, valgrind, nano, vim
- ✅ **All 11 examples pre-compiled** and ready to run!
- ✅ **Helper scripts**: `examples`, `run-all`, `benchmark`, `compile-single`

## 🚀 Quick Start (3 Steps)

### 1. Build the Docker Image

```bash
cd examples/
docker-compose build
```

This creates the image with all dependencies and compiles all examples.

### 2. Start the Container

```bash
docker-compose up -d
```

Starts the container in detached mode (runs in background).

### 3. Enter the Container

```bash
docker-compose exec parallel-examples bash
```

You'll see a welcome screen with all available commands!

## 📋 Commands Inside the Container

Once inside, you have these convenient commands:

| Command | What it does |
|---------|--------------|
| `examples` | Lists all 11 pre-compiled examples |
| `run-all` | Runs all examples with 5-second timeout each |
| `benchmark` | Performance benchmarks of key examples |
| `compile-single file.cpp` | Compiles a single file (auto-detects C++17/20) |

## 🎯 Example Session

```bash
# On your host machine
$ cd examples/
$ docker-compose up -d
$ docker-compose exec parallel-examples bash

# Now you're inside the container
root@abc123:/examples# examples
=== Parallel Programming Examples ===

Available examples:
  1. Lock-based thread pool:      ./build/01_thread_pool_lock_based
  2. Lock-free queue:             ./build/02_lock_free_queue
  ...

root@abc123:/examples# ./build/01_thread_pool_lock_based
=== Lock-based Thread Pool Example ===
Hardware concurrency: 8
...

root@abc123:/examples# benchmark
=== Performance Benchmarks ===
System info:
  CPU cores: 8
  Memory: 2.0Gi
...

root@abc123:/examples# compile-single 01_thread_pool_lock_based.cpp my_pool
Using C++17
Successfully compiled: my_pool
Run with: ./my_pool

root@abc123:/examples# ./my_pool
=== Lock-based Thread Pool Example ===
...

root@abc123:/examples# exit
```

## 🔧 Advanced Usage

### Run Single Example Without Entering Shell

```bash
docker-compose exec parallel-examples ./build/07_atomic_memory_ordering
```

### Edit Code and Rebuild

The container has a volume mount, so you can:

1. **Edit files on your host** with your favorite editor
2. **Rebuild inside container**:
   ```bash
   docker-compose exec parallel-examples bash
   cd build && ninja
   ```
3. **Run updated example**

### Use Different Compiler

Inside container:
```bash
export CXX=clang++
cd build
cmake -G Ninja -DCMAKE_CXX_COMPILER=clang++ ..
ninja
```

### Debugging with GDB

```bash
# Inside container
g++ -g -pthread 01_thread_pool_lock_based.cpp -o debug_pool
gdb ./debug_pool
(gdb) run
(gdb) bt
```

### Memory Check with Valgrind

```bash
# Inside container
valgrind --leak-check=full ./build/02_lock_free_queue
```

### Thread Sanitizer

```bash
# Inside container
g++ -std=c++17 -pthread -fsanitize=thread -g 01_thread_pool_lock_based.cpp -o tsan
./tsan
```

## 📊 System Information

Inside the container, check system resources:

```bash
nproc          # Number of CPU cores
free -h        # Memory info
g++ --version  # Compiler version
lscpu          # Detailed CPU info
htop           # Interactive process viewer
```

## 🛑 Stopping and Cleanup

### Stop Container (keep data)
```bash
docker-compose down
```

### Remove Everything (including volumes)
```bash
docker-compose down -v
docker rmi parallel-programming-examples
```

### Restart Container
```bash
docker-compose restart
```

## ⚙️ Configuration

### Adjust CPU Cores

Edit `docker-compose.yml`:
```yaml
services:
  parallel-examples:
    cpus: '4'  # Limit to 4 cores
```

### Adjust Memory

Edit `docker-compose.yml`:
```yaml
services:
  parallel-examples:
    mem_limit: 4g  # Increase to 4GB
```

## 🐛 Troubleshooting

### Container won't start

```bash
docker-compose logs
docker-compose build --no-cache
docker-compose up -d
```

### Permission denied

```bash
# On Linux, may need to adjust ownership
sudo chown -R $USER:$USER .
```

### Can't find compiled examples

```bash
# Inside container, rebuild
cd /examples/build
cmake -G Ninja ..
ninja
```

## 📚 Documentation Files

- **DOCKER.md** - This file (complete Docker guide)
- **README.md** - Full examples documentation
- **QUICK_REFERENCE.md** - Quick lookup guide
- **Dockerfile** - Image definition
- **docker-compose.yml** - Container orchestration

## 💡 Tips & Best Practices

1. **Use volumes** - Your code changes persist and are visible inside container
2. **Pre-built** - All examples are already compiled on first build
3. **Isolated** - Container provides consistent environment across machines
4. **No cleanup needed** - Everything is in the container, host stays clean
5. **Reproducible** - Same environment on Mac/Windows/Linux

## 🎓 Learning Path with Docker

### Beginner
```bash
docker-compose exec parallel-examples ./build/01_thread_pool_lock_based
docker-compose exec parallel-examples ./build/04_pubsub_synchronous
```

### Intermediate
```bash
docker-compose exec parallel-examples ./build/02_lock_free_queue
docker-compose exec parallel-examples ./build/07_atomic_memory_ordering
```

### Advanced
```bash
docker-compose exec parallel-examples ./build/06_pubsub_lockfree_rcu
docker-compose exec parallel-examples ./build/10_hybrid_approach
```

## 🌐 Why Docker?

✅ **No dependency hell** - Everything pre-installed  
✅ **Works everywhere** - Mac, Windows, Linux  
✅ **Consistent environment** - Same compiler versions  
✅ **Isolated** - Won't mess up your system  
✅ **Quick setup** - 3 commands and you're ready  
✅ **Pre-built examples** - No waiting for compilation  
✅ **Easy cleanup** - `docker-compose down` removes everything  

## 🔗 Alternative: Local Setup

If you prefer not to use Docker, run:
```bash
./setup.sh
```

This script will:
- Check your compiler version
- Install dependencies (asks for confirmation)
- Build all examples with CMake
- Provide compilation instructions

## 📞 Support

For questions or issues:
- Email: atsvetanov@tu-sofia.bg
- Check DOCKER.md for detailed troubleshooting
- See README.md for examples documentation

---

**Happy Parallel Programming! 🚀**
