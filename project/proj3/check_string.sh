#! /bin/sh

# Trust me.
strings --all $1 | sed 's/\t/\n/g' | grep -P '.{4,}' > strings.out
./mystrings $1 > mystrings.out
git diff --color strings.out mystrings.out