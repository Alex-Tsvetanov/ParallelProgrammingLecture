# Dockerfile for Parallel Programming Examples
# Provides a complete environment with C++20 support and all dependencies

FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Set up locale
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++-12 \
    clang-14 \
    libtbb-dev \
    git \
    ninja-build \
    gdb \
    valgrind \
    nano \
    vim \
    htop \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 12 as default (for C++20 support)
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# Set Clang 14 as alternative compiler
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-14 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-14 100

# Create working directory
WORKDIR /examples

# Copy all example files
COPY *.cpp ./
COPY CMakeLists.txt ./
COPY README.md ./
COPY QUICK_REFERENCE.md ./

# Pre-build all examples with CMake
RUN mkdir -p build && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. \
    && ninja

# Create convenience scripts
RUN echo '#!/bin/bash\n\
echo "=== Parallel Programming Examples ==="\n\
echo ""\n\
echo "Available examples:"\n\
echo "  1. Lock-based thread pool:      ./build/01_thread_pool_lock_based"\n\
echo "  2. Lock-free queue:             ./build/02_lock_free_queue"\n\
echo "  3. Basic coroutine:             ./build/03_basic_coroutine"\n\
echo "  4. Synchronous pub/sub:         ./build/04_pubsub_synchronous"\n\
echo "  5. Async pub/sub + thread pool: ./build/05_pubsub_async_threadpool"\n\
echo "  6. Lock-free pub/sub (RCU):     ./build/06_pubsub_lockfree_rcu"\n\
echo "  7. Atomic operations:           ./build/07_atomic_memory_ordering"\n\
echo "  8. OneTBB examples:             ./build/08_onetbb_examples"\n\
echo "  9. Coroutine async I/O:         ./build/09_coroutine_async_io"\n\
echo " 10. Hybrid approach:             ./build/10_hybrid_approach"\n\
echo " 11. Coroutine thread pool:       ./build/coroutine_based_thread_pool"\n\
echo ""\n\
echo "Documentation:"\n\
echo "  - README.md - Full documentation"\n\
echo "  - QUICK_REFERENCE.md - Quick reference guide"\n\
echo ""\n\
echo "To rebuild:"\n\
echo "  cd build && ninja"\n\
echo ""\n\
echo "To compile single file:"\n\
echo "  g++ -std=c++20 -pthread -O2 <file>.cpp -o <output>"\n\
echo ""\n\
' > /usr/local/bin/examples && chmod +x /usr/local/bin/examples

# Create run-all script
RUN echo '#!/bin/bash\n\
echo "=== Running All Examples ==="\n\
echo ""\n\
for exe in build/*; do\n\
    if [ -x "$exe" ] && [ ! -d "$exe" ]; then\n\
        echo ""\n\
        echo "========================================"\n\
        echo "Running: $exe"\n\
        echo "========================================"\n\
        timeout 5s "$exe" || echo "Timed out or failed"\n\
    fi\n\
done\n\
echo ""\n\
echo "=== All examples completed ==="\n\
' > /usr/local/bin/run-all && chmod +x /usr/local/bin/run-all

# Create benchmark script
RUN echo '#!/bin/bash\n\
echo "=== Performance Benchmarks ==="\n\
echo ""\n\
echo "System info:"\n\
echo "  CPU cores: $(nproc)"\n\
echo "  Memory: $(free -h | grep Mem | awk '\''{print $2}'\'')"\n\
echo "  Compiler: $(g++ --version | head -1)"\n\
echo ""\n\
\n\
run_benchmark() {\n\
    local name=$1\n\
    local exe=$2\n\
    echo ""\n\
    echo "--- $name ---"\n\
    if [ -x "$exe" ]; then\n\
        time "$exe" > /dev/null 2>&1\n\
    else\n\
        echo "  Not found or not executable"\n\
    fi\n\
}\n\
\n\
run_benchmark "Thread Pool (lock-based)" "build/01_thread_pool_lock_based"\n\
run_benchmark "Lock-Free Queue" "build/02_lock_free_queue"\n\
run_benchmark "Atomic Operations" "build/07_atomic_memory_ordering"\n\
run_benchmark "OneTBB Examples" "build/08_onetbb_examples"\n\
run_benchmark "Hybrid Approach" "build/10_hybrid_approach"\n\
\n\
echo ""\n\
echo "=== Benchmarks completed ==="\n\
' > /usr/local/bin/benchmark && chmod +x /usr/local/bin/benchmark

# Create compile-single script
RUN echo '#!/bin/bash\n\
if [ $# -eq 0 ]; then\n\
    echo "Usage: compile-single <filename.cpp> [output_name]"\n\
    echo "Example: compile-single 01_thread_pool_lock_based.cpp my_pool"\n\
    exit 1\n\
fi\n\
\n\
INPUT=$1\n\
OUTPUT=${2:-${INPUT%.cpp}}\n\
\n\
# Detect if coroutine example (C++20)\n\
if grep -q "co_await\\|co_return\\|co_yield" "$INPUT"; then\n\
    echo "Detected coroutine - using C++20"\n\
    g++ -std=c++20 -pthread -O2 -Wall -Wextra "$INPUT" -o "$OUTPUT"\n\
elif grep -q "#include.*tbb" "$INPUT"; then\n\
    echo "Detected TBB - linking against TBB"\n\
    g++ -std=c++17 -pthread -O2 -Wall -Wextra "$INPUT" -ltbb -o "$OUTPUT"\n\
else\n\
    echo "Using C++17"\n\
    g++ -std=c++17 -pthread -O2 -Wall -Wextra "$INPUT" -o "$OUTPUT"\n\
fi\n\
\n\
if [ $? -eq 0 ]; then\n\
    echo "Successfully compiled: $OUTPUT"\n\
    echo "Run with: ./$OUTPUT"\n\
else\n\
    echo "Compilation failed"\n\
    exit 1\n\
fi\n\
' > /usr/local/bin/compile-single && chmod +x /usr/local/bin/compile-single

# Set environment variables
ENV CXX=g++
ENV CC=gcc
ENV MAKEFLAGS="-j$(nproc)"

# Print welcome message on container start
RUN echo '#!/bin/bash\n\
clear\n\
echo "╔════════════════════════════════════════════════════════════════╗"\n\
echo "║     Parallel Programming Examples - Docker Environment        ║"\n\
echo "╚════════════════════════════════════════════════════════════════╝"\n\
echo ""\n\
echo "📚 Documentation:"\n\
echo "   - README.md (full guide)"\n\
echo "   - QUICK_REFERENCE.md (quick reference)"\n\
echo ""\n\
echo "🚀 Quick Commands:"\n\
echo "   examples        - List all examples"\n\
echo "   run-all         - Run all examples (with timeout)"\n\
echo "   benchmark       - Performance benchmarks"\n\
echo "   compile-single  - Compile a single file"\n\
echo ""\n\
echo "🔧 Example Commands:"\n\
echo "   ./build/01_thread_pool_lock_based"\n\
echo "   ./build/07_atomic_memory_ordering"\n\
echo "   compile-single 01_thread_pool_lock_based.cpp my_program"\n\
echo ""\n\
echo "📖 View documentation:"\n\
echo "   cat README.md"\n\
echo "   cat QUICK_REFERENCE.md"\n\
echo ""\n\
echo "💡 System Info:"\n\
echo "   CPU cores: $(nproc)"\n\
echo "   Compiler: $(g++ --version | head -1)"\n\
echo "   CMake: $(cmake --version | head -1)"\n\
echo "   TBB: $(dpkg -l | grep libtbb | awk '\''{print $2,$3}'\'')"\n\
echo ""\n\
exec /bin/bash\n\
' > /usr/local/bin/welcome && chmod +x /usr/local/bin/welcome

# Set welcome as default command
CMD ["/usr/local/bin/welcome"]

# Labels
LABEL maintainer="atsvetanov@tu-sofia.bg"
LABEL description="Parallel Programming Examples - TU Sofia Course"
LABEL version="1.0"

# Expose no ports (these are computational examples)
EXPOSE 0

# Health check (verify build directory exists and has executables)
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=1 \
    CMD [ -d "/examples/build" ] && [ "$(ls -1 /examples/build/* 2>/dev/null | wc -l)" -gt "5" ] || exit 1
