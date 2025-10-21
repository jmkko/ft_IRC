cat test/debug_ci.sh 
#!/bin/bash

# Script pour débugger les différences entre environnement local et CI

echo "=== Debug CI vs Local Environment ==="
echo "OS: $(uname -s)"
echo "Kernel: $(uname -r)"
echo "Compiler: $(clang++ --version | head -n1)"
echo "Available memory: $(free -h 2>/dev/null || vm_stat 2>/dev/null | head -n1)"
echo "CPU count: $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null)"

echo -e "\n=== Network configuration ==="
echo "Listening on port 4343:"
netstat -ln 2>/dev/null | grep 4343 || lsof -i :4343 2>/dev/null
echo "Network interfaces:"
ifconfig 2>/dev/null | grep -E "(^[a-z]|inet )" | head -10

echo -e "\n=== Building and running tests with verbose output ==="
make re
echo "Running test with debug output:"
DEB=1 ./test 2>&1 | tee test_debug_output.log