# Leetdev

My personal boilerplate setup for local development and debugging solutions for leetcode problems (in C)

## Files

`.clang-format` - settings for `clang-format`. Tuned for my personal likings

`common.h` - some of useful (at least for me) utils

`compile_commands.json` - needed for clang to work properly as language server
If you update/change flags for compiler you need to update this file by running `bear -- make` or `make lsp`

`dap.lua` - setup for debugging in neovim

`solution.c` - the file with solution

`update.c` - prgram that updates `main.c` for new task
It will auto-generate `Testcase` structure with proper types and arguments

## How to update

1. Remove your solution funcion
2. Paste deafult empty function from leetcode code editor
3. Run `./update`
4. Fill `testcases` array
5. ???
6. Get profit!
