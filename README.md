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
 like
``` 
python test.py <bondriver path> <spaceNo> <channel> <time> <outfile>
```
 <outfile> is created if success.


