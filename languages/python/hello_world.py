
# create executable binary from python code, toy example
# inspired by http://masnun.rocks/2016/10/01/creating-an-executable-file-using-cython/
# cython --embed -o hello_world.c hello_world.py
# gcc -v -Os \
#      -L /usr/local/Frameworks/Python.framework/Versions/3.7/lib/ \
#      -I /usr/local/Frameworks/Python.framework/Versions/3.7/include/python3.7m/ \
#      -lpython3.7  -lpthread -lm -lutil -ldl 
#      -o hello \
#      hello_world.c


#gcc -v -Os -I /Users/xxx/.pyenv/versions/3.5.1/include/python3.5m -L /usr/local/Frameworks/Python.framework/Versions/3.5/lib  -o test test.c  -lpython3.5  -lpthread -lm -lutil -ldl

print("Hello World!")

