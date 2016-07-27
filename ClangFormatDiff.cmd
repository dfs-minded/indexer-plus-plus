echo Starting Clang Format on git diff sections.

git diff -U0 HEAD^ | clang-format-diff.py -i -p1

echo Clang Format finished. 