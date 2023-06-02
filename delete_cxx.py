import glob, os

for file in os.listdir():
    if file.endswith(".cxx"):
        os.remove(file)
        print(file)