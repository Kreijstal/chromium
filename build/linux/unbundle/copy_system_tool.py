import shutil, sys
shutil.copy2(shutil.which(sys.argv[1]), sys.argv[2])
