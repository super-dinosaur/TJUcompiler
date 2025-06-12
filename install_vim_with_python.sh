#!/bin/bash

echo "Installing Vim with Python3 support..."

# Install MacVim which includes Python support
brew install macvim

# Create alias for vim to use mvim
echo "alias vim='mvim -v'" >> ~/.zshrc

# Or install Neovim (recommended - has built-in Python support)
brew install neovim
brew install python3
pip3 install pynvim

echo "Installation complete!"
echo "For MacVim: Use 'mvim -v' or add the alias to use 'vim'"
echo "For Neovim: Use 'nvim' command"
echo ""
echo "To use Neovim with your current config:"
echo "ln -s ~/.vimrc ~/.config/nvim/init.vim"
