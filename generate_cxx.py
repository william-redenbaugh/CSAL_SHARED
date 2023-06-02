import glob, os

for file in os.listdir():
    if file.endswith(".cpp"):
        print(file)
        f = open(file, 'r')
        fx = open(file[:-3] + "cxx", 'w')
        fx.writelines(f.readlines())