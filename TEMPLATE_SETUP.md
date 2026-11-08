# Template setup

Checklist for a new project created from this template.
**Delete this file once done**: it is not part of the project.

## 1. Rename

- [ ] `Makefile`: set `NAME` to the binary name the subject asks for
- [ ] `.gitignore`: replace `template` with that same name
- [ ] `includes/template.h`: rename the file (e.g. `push_swap.h`), its guard
      (`TEMPLATE_H`), the `HEADERS` entry in the `Makefile`,
      and the `#include` in `main.c`
- [ ] Regenerate the 42 header of every file you touched
- [ ] `main.c`: replace the gnl/ft_printf demo with the real entry point,
      and delete `test.txt`

## 2. README

- [ ] Fill in the placeholders (`NAME`, description, instructions, subject
      version, AI section)

## 3. Layout and include rules

- Headers go in `includes/`, sources anywhere (`parsing/`, `moves/`...)
- Every `.c` includes only the project header, by bare name:
  `#include "push_swap.h"`, never `"../includes/push_swap.h"`
- The `Makefile` passes `-Iincludes -Ilibft`, so bare names resolve everywhere
- If a header only needs your types, give the types their own header
  (e.g. `structs.h`) to avoid circular includes

## 4. Editor (red squiggles)

`compile_flags.txt` tells **clangd** where the headers are
(`-Iincludes -Ilibft`). It works out of the box with this layout.

- Added another include folder? Add a `-I<folder>` line to it.
- For exact flags on a bigger project: `bear -- make re` generates
  `compile_commands.json` (git-ignored, machine-specific).
- Microsoft C/C++ extension instead of clangd: add `${workspaceFolder}/includes`
  and `${workspaceFolder}/libft` to `includePath` in
  `.vscode/c_cpp_properties.json`.

## 5. Before copying to vogsphere

- [ ] Remove `compile_flags.txt` (and this file, if still there)
- [ ] `make fclean`: no binaries or `.o` in the copy
- [ ] `make re` builds with no warnings, `norminette` passes
