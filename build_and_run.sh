#!/bin/bash
# Quick build and run script for your CmmCompiler project

echo "🔨 Building CmmCompiler..."
cd /Users/franky/Documents/code/CmmCompiler

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo "📋 Configuring with CMake..."
cmake ..

# Build the project
echo "🏗️  Compiling..."
make

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "📍 Executable location: ./CmmCompiler"
    echo ""
    echo "To run your compiler:"
    echo "  ./CmmCompiler"
    echo "  ./CmmCompiler ../test/test_1.sy"
else
    echo "❌ Build failed!"
fi
