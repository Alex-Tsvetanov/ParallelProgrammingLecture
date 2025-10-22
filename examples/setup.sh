#!/bin/bash
# Quick setup and build script for Parallel Programming Examples

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║     Parallel Programming Examples - Setup Script              ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Check for Docker
if command -v docker &> /dev/null && command -v docker-compose &> /dev/null; then
    echo -e "${GREEN}✓${NC} Docker and Docker Compose found"
    echo ""
    echo "Would you like to use Docker? (recommended)"
    echo "  1) Yes - Use Docker (no local setup needed)"
    echo "  2) No  - Compile locally"
    read -p "Enter choice [1-2]: " choice

    if [ "$choice" = "1" ]; then
        echo ""
        echo "Building Docker image..."
        docker-compose build

        echo ""
        echo -e "${GREEN}✓${NC} Docker image built successfully!"
        echo ""
        echo "To start:"
        echo "  docker-compose up -d          # Start container"
        echo "  docker-compose exec parallel-examples bash  # Enter container"
        echo ""
        echo "See DOCKER.md for more information"
        exit 0
    fi
fi

echo ""
echo "Setting up local compilation..."
echo ""

# Check for C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo -e "${RED}✗${NC} No C++ compiler found!"
    echo "Please install g++ or clang++"
    echo ""
    echo "Ubuntu/Debian: sudo apt-get install build-essential"
    echo "macOS: xcode-select --install"
    exit 1
fi

# Detect compiler and version
if command -v g++ &> /dev/null; then
    COMPILER="g++"
    VERSION=$(g++ --version | head -1 | grep -oP '\d+\.\d+' | head -1)
    MAJOR=$(echo $VERSION | cut -d. -f1)

    echo -e "${GREEN}✓${NC} Found g++ version $VERSION"

    if [ "$MAJOR" -lt 10 ]; then
        echo -e "${YELLOW}⚠${NC} Warning: g++ $VERSION may not support C++20 coroutines"
        echo "  Recommended: g++ 10 or higher"
        echo "  Coroutine examples may not compile"
    fi
elif command -v clang++ &> /dev/null; then
    COMPILER="clang++"
    VERSION=$(clang++ --version | head -1 | grep -oP '\d+\.\d+' | head -1)
    MAJOR=$(echo $VERSION | cut -d. -f1)

    echo -e "${GREEN}✓${NC} Found clang++ version $VERSION"

    if [ "$MAJOR" -lt 11 ]; then
        echo -e "${YELLOW}⚠${NC} Warning: clang++ $VERSION may not support C++20 coroutines"
        echo "  Recommended: clang++ 11 or higher"
    fi
fi

# Check for CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -1 | grep -oP '\d+\.\d+\.\d+')
    echo -e "${GREEN}✓${NC} Found CMake version $CMAKE_VERSION"
else
    echo -e "${YELLOW}⚠${NC} CMake not found (optional)"
    echo "  You can still compile individual files"
    echo "  To install: sudo apt-get install cmake (Ubuntu/Debian)"
fi

# Check for TBB
if ldconfig -p 2>/dev/null | grep -q libtbb || [ -f /usr/lib/libtbb.so ] || [ -f /usr/local/lib/libtbb.so ]; then
    echo -e "${GREEN}✓${NC} Found oneTBB library"
elif [ "$(uname)" = "Darwin" ] && [ -d /usr/local/opt/tbb ]; then
    echo -e "${GREEN}✓${NC} Found oneTBB library (Homebrew)"
else
    echo -e "${YELLOW}⚠${NC} oneTBB not found (optional)"
    echo "  Example 08 won't compile without it"
    echo "  To install:"
    echo "    Ubuntu/Debian: sudo apt-get install libtbb-dev"
    echo "    macOS: brew install tbb"
fi

echo ""
echo "Checking for pthread support..."
echo "#include <thread>" > /tmp/test_thread.cpp
echo "int main() { return 0; }" >> /tmp/test_thread.cpp

if $COMPILER -std=c++17 -pthread /tmp/test_thread.cpp -o /tmp/test_thread 2>/dev/null; then
    echo -e "${GREEN}✓${NC} pthread support available"
    rm -f /tmp/test_thread.cpp /tmp/test_thread
else
    echo -e "${RED}✗${NC} pthread support missing"
    rm -f /tmp/test_thread.cpp
    exit 1
fi

echo ""
echo "════════════════════════════════════════════════════════════════"
echo ""

# Try to build with CMake
if command -v cmake &> /dev/null; then
    echo "Would you like to build all examples with CMake?"
    read -p "Build now? [Y/n]: " build_choice

    if [ "$build_choice" != "n" ] && [ "$build_choice" != "N" ]; then
        echo ""
        echo "Building all examples..."
        mkdir -p build
        cd build

        if cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc 2>/dev/null || echo 4); then
            echo ""
            echo -e "${GREEN}✓${NC} Build successful!"
            echo ""
            echo "Executables are in: $SCRIPT_DIR/build/"
            echo ""
            echo "Run examples:"
            cd ..
            for exe in build/*; do
                if [ -x "$exe" ] && [ ! -d "$exe" ]; then
                    echo "  ./$exe"
                fi
            done
        else
            echo ""
            echo -e "${YELLOW}⚠${NC} CMake build had some issues"
            echo "You can still compile individual files manually"
        fi
    fi
else
    echo "To compile an individual example:"
    echo "  g++ -std=c++17 -pthread -O2 01_thread_pool_lock_based.cpp -o thread_pool"
    echo "  ./thread_pool"
    echo ""
    echo "For coroutine examples (C++20):"
    echo "  g++ -std=c++20 -pthread -O2 03_basic_coroutine.cpp -o basic_coroutine"
    echo ""
    echo "For oneTBB example:"
    echo "  g++ -std=c++17 -pthread -O2 08_onetbb_examples.cpp -ltbb -o tbb_example"
fi

echo ""
echo "════════════════════════════════════════════════════════════════"
echo ""
echo "📚 Documentation:"
echo "  - README.md - Full documentation"
echo "  - QUICK_REFERENCE.md - Quick reference"
echo "  - DOCKER.md - Docker instructions"
echo ""
echo "Setup complete! Happy coding! 🚀"
echo ""
