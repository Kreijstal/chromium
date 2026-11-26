import sys
for p in sys.argv[1:]:
    open(p, "a").close()
