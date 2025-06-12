#!/bin/bash

# Setup script for C/C++ development environment on macOS with Apple Silicon
# Run this script with: bash setup_dev_env.sh

echo "Setting up C/C++ development environment..."

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    # Add Homebrew to PATH for Apple Silicon
    echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zshrc
    eval "$(/opt/homebrew/bin/brew shellenv)"
fi

# Update Homebrew
echo "Updating Homebrew..."
brew update

# Install development tools
echo "Installing development tools..."
brew install cmake
brew install ninja
brew install llvm
brew install clang-format
brew install universal-ctags
brew install ripgrep
brew install fzf
brew install node  # Needed for coc.nvim
brew install ccls  # Alternative to clangd
brew install codelldb  # Debugger for vimspector

# Install Vim/Neovim
echo "Installing Neovim..."
brew install neovim

# Create necessary directories
echo "Creating necessary directories..."
mkdir -p ~/.vim/undodir
mkdir -p ~/.vim/UltiSnips
mkdir -p ~/.config/nvim

# Install vim-plug
echo "Installing vim-plug..."
curl -fLo ~/.vim/autoload/plug.vim --create-dirs \
    https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim

# Create symlink for Neovim config
if [ ! -f ~/.config/nvim/init.vim ]; then
    ln -s ~/.vimrc ~/.config/nvim/init.vim
fi

# Install CoC extensions
echo "Installing CoC extensions..."
vim +PlugInstall +qall
vim -c 'CocInstall -sync coc-clangd coc-cmake coc-json coc-snippets coc-pairs coc-git coc-explorer coc-highlight coc-diagnostic' +qall

# Setup compile_commands.json generation for your project
echo "Setting up compile_commands.json generation..."
cd /Users/franky/Documents/code/CmmCompiler
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cd ..
ln -sf build/compile_commands.json .

# Install additional tools for better experience
echo "Installing additional tools..."
brew install bear  # For generating compile_commands.json
brew install cppcheck  # Static analysis tool
brew install doxygen  # Documentation generator
brew install graphviz  # For doxygen graphs

# Setup clangd
echo "Setting up clangd..."
if [ ! -f ~/.config/clangd/config.yaml ]; then
    mkdir -p ~/.config/clangd
    cat > ~/.config/clangd/config.yaml << 'EOF'
CompileFlags:
  Add: [-std=c++17, -Wall, -Wextra]
  Remove: -W*
Diagnostics:
  ClangTidy:
    Add: [performance-*, modernize-*, readability-*]
    Remove: [modernize-use-trailing-return-type]
EOF
fi

# Create a basic .gitignore for C++ projects
echo "Creating .gitignore..."
cat > /Users/franky/Documents/code/CmmCompiler/.gitignore << 'EOF'
# Build directories
build/
cmake-build-*/
out/

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Precompiled Headers
*.gch
*.pch

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Fortran module files
*.mod
*.smod

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

# IDE and editor files
.vscode/
.idea/
*.swp
*.swo
*~
.DS_Store

# Debugging files
*.dSYM/
*.su
*.idb
*.pdb

# CMake
CMakeLists.txt.user
CMakeCache.txt
CMakeFiles
CMakeScripts
Testing
Makefile
cmake_install.cmake
install_manifest.txt
compile_commands.json
CTestTestfile.cmake
_deps

# Vim
.vim/
tags
.tags

# Clangd
.clangd/
.cache/
EOF

echo "Setup complete! Please restart your terminal and run 'vim +PlugInstall' to install Vim plugins."
echo ""
echo "Quick tips:"
echo "1. Use 'brew info llvm' to see how to add LLVM to your PATH if needed"
echo "2. Run 'vim' and execute ':checkhealth' to verify everything is working"
echo "3. In your project directory, run 'cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .' to generate compile_commands.json"
echo "4. Use <Space>nt to toggle file tree, <C-p> to search files, <F5> to compile and run"
