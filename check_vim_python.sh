#!/bin/bash

echo "Checking Vim Python support..."
vim --version | grep -E "python|Python"

echo -e "\nChecking if Python3 is available:"
which python3

echo -e "\nChecking Neovim (if installed):"
if command -v nvim &> /dev/null; then
    nvim --version | head -n 1
    echo "Neovim Python support:"
    nvim -c ':echo has("python3")' -c ':q' 2>/dev/null
else
    echo "Neovim not installed"
fi
