# Docker Usage Guide

## Quick Start

### Option 1: Using Docker Compose (Recommended)

```bash
# Build and start the container
docker-compose up -d

# Enter the container
docker-compose exec parallel-examples bash

# Inside the container, you'll see the welcome screen with instructions
```

### Option 2: Using Docker directly

```bash
# Build the image
docker build -t parallel-programming-examples .

# Run the container interactively
docker run -it --rm parallel-programming-examples

# Or run with mounted volume for live editing
docker run -it --rm -v $(pwd):/examples parallel-programming-examples
```

## Available Commands Inside Container

### Quick Commands

| Command | Description |
|---------|-------------|
| `examples` | List all available examples |
| `run-all` | Run all examples with timeout |
| `benchmark` | Run performance benchmarks |
| `compile-single <file.cpp>` | Compile a single file |

### Examples

```bash
# List all examples
examples

# Run a specific example
./build/01_thread_pool_lock_based
./build/07_atomic_memory_ordering
./build/08_onetbb_examples

# Run all examples
run-all

# Performance benchmark
benchmark

# Compile a single file
compile-single 01_thread_pool_lock_based.cpp my_program
./my_program

# Read documentation
cat README.md
cat QUICK_REFERENCE.md
```

## Development Workflow

### Edit and Rebuild

```bash
# 1. Start container with volume mount
docker-compose up -d
docker-compose exec parallel-examples bash

# 2. Edit files (from host or inside container)
vim 01_thread_pool_lock_based.cpp

# 3. Rebuild
cd build
ninja

# 4. Run updated example
./01_thread_pool_lock_based
```

### Compile Individual Files

```bash
# C++17 example
g++ -std=c++17 -pthread -O2 01_thread_pool_lock_based.cpp -o my_pool
./my_pool

# C++20 coroutine example
g++ -std=c++20 -pthread -O2 03_basic_coroutine.cpp -o my_coro
./my_coro

# OneTBB example
g++ -std=c++17 -pthread -O2 08_onetbb_examples.cpp -ltbb -o my_tbb
./my_tbb

# Or use the helper script
compile-single 01_thread_pool_lock_based.cpp
```

## Debugging

### Using GDB

```bash
# Compile with debug symbols
g++ -std=c++17 -pthread -g 01_thread_pool_lock_based.cpp -o pool_debug

# Run with GDB
gdb ./pool_debug
(gdb) run
(gdb) bt  # backtrace
(gdb) info threads
```

### Using Valgrind

```bash
# Check for memory leaks
valgrind --leak-check=full ./build/01_thread_pool_lock_based

# Check for race conditions (Helgrind)
valgrind --tool=helgrind ./build/02_lock_free_queue

# Check for data races (DRD)
valgrind --tool=drd ./build/05_pubsub_async_threadpool
```

### Thread Sanitizer

```bash
# Compile with thread sanitizer
g++ -std=c++17 -pthread -g -fsanitize=thread 01_thread_pool_lock_based.cpp -o pool_tsan
./pool_tsan
```

## Docker Management

### Container Lifecycle

```bash
# Start container
docker-compose up -d

# Stop container
docker-compose down

# View logs
docker-compose logs -f

# Restart container
docker-compose restart

# Remove container and volumes
docker-compose down -v
```

### Image Management

```bash
# Build image
docker-compose build

# Rebuild without cache
docker-compose build --no-cache

# Remove image
docker rmi parallel-programming-examples

# View images
docker images | grep parallel
```

### Execute Commands Without Entering Shell

```bash
# Run a specific example
docker-compose exec parallel-examples ./build/01_thread_pool_lock_based

# List examples
docker-compose exec parallel-examples examples

# Run benchmarks
docker-compose exec parallel-examples benchmark
```

## Customization

### Change Number of CPU Cores

Edit `docker-compose.yml`:
```yaml
services:
  parallel-examples:
    cpus: '4'  # Limit to 4 cores
```

### Change Memory Limit

Edit `docker-compose.yml`:
```yaml
services:
  parallel-examples:
    mem_limit: 4g  # Increase to 4GB
```

### Use Different Compiler

Inside container:
```bash
# Use Clang instead of GCC
export CXX=clang++
export CC=clang

# Rebuild
cd build
cmake -G Ninja -DCMAKE_CXX_COMPILER=clang++ ..
ninja
```

## Troubleshooting

### Container won't start

```bash
# Check logs
docker-compose logs

# Rebuild from scratch
docker-compose down
docker-compose build --no-cache
docker-compose up -d
```

### Permission issues with mounted volumes

```bash
# On Linux, you may need to adjust permissions
chmod -R 755 .
```

### Out of memory errors

```bash
# Increase memory limit in docker-compose.yml
mem_limit: 4g

# Or check Docker Desktop settings (Mac/Windows)
```

### Examples won't compile

```bash
# Inside container, rebuild
cd /examples/build
rm -rf *
cmake -G Ninja ..
ninja

# Or use compile-single for individual files
compile-single 01_thread_pool_lock_based.cpp
```

## Performance Testing

### System Information

```bash
# Inside container
nproc              # Number of CPU cores
free -h            # Memory info
g++ --version      # Compiler version
lscpu              # Detailed CPU info
```

### Running Benchmarks

```bash
# Quick benchmark
benchmark

# Detailed timing
time ./build/10_hybrid_approach

# With profiling
perf record ./build/07_atomic_memory_ordering
perf report
```

## Advanced Usage

### Multi-stage Development

```bash
# Terminal 1: Keep container running
docker-compose up

# Terminal 2: Execute commands
docker-compose exec parallel-examples bash

# Terminal 3: Watch logs
docker-compose logs -f
```

### Copy Files From Container

```bash
# Copy compiled binary out
docker cp parallel-programming:/examples/build/01_thread_pool_lock_based ./my_pool
```

### Network Between Containers

If you want to test distributed examples:

```yaml
# In docker-compose.yml
services:
  node1:
    build: .
    networks:
      - parallel-net
  node2:
    build: .
    networks:
      - parallel-net

networks:
  parallel-net:
    driver: bridge
```

## Clean Up

### Remove Everything

```bash
# Stop and remove containers, networks, volumes
docker-compose down -v

# Remove images
docker rmi parallel-programming-examples

# Clean Docker system
docker system prune -a
```

## Production Deployment

### Export Examples

```bash
# Build optimized versions
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja

# Copy to host
docker cp parallel-programming:/examples/build ./compiled-examples
```

### Create Minimal Runtime Image

```dockerfile
FROM ubuntu:22.04
COPY --from=builder /examples/build/* /usr/local/bin/
# ... minimal runtime dependencies only
```

## Tips & Best Practices

1. **Use volumes** for development to edit files on host
2. **Limit resources** (CPU/memory) to simulate production
3. **Use thread sanitizer** to catch race conditions
4. **Benchmark on same hardware** for consistent results
5. **Clean rebuild** if seeing strange behavior

## Support

For issues or questions:
- Email: atsvetanov@tu-sofia.bg
- GitHub: https://github.com/Alex-Tsvetanov/ParallelProgrammingLecture

## Example Session

```bash
# Start everything
$ docker-compose up -d
$ docker-compose exec parallel-examples bash

# Inside container
root@container:/examples# examples
=== Parallel Programming Examples ===
...

root@container:/examples# ./build/01_thread_pool_lock_based
=== Lock-based Thread Pool Example ===
...

root@container:/examples# benchmark
=== Performance Benchmarks ===
...

root@container:/examples# compile-single 01_thread_pool_lock_based.cpp my_custom
Successfully compiled: my_custom

root@container:/examples# ./my_custom
...

root@container:/examples# exit

# Stop container
$ docker-compose down
```
