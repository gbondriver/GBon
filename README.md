BonDriver GObject wrapper
===

## How to build
Install many package like gob2, gobject-introspect, libgirepository, etc.
```
./autogen.sh
./configure --prefix=/usr --enable-b25
make
sudo make install
```
## How to test
 edit test.py. BonDriver's Path and channel.
 and
``` 
python test.py
```
 test.ts file is created if success.


