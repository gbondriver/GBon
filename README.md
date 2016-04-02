BonDriver GObject wrapper
===

  BonDriverのGObjectへのラッパーです。  
  python等で、録画とか視聴とかのプログラムを書きやすくするのがねらいです。

## BUild

  gcc, g++, autoconf, gob2, gobject-introspection, libgirepositoryなどの開発用のパッケージを入れます。　　
  BonDriverProxy_Linuxのソースも必要です。同じ階層に並べて下さい。  
  libaribは、任意です。  
  そしたら、
  
```
./autogen.sh
./configure --prefix=/usr --enable-b25
make
sudo make install
```

  です。 --prefix=/usrを指定したほうが、何かと楽です。

## Test

  録画テストは  
  
``` 
python test.py <bondriver> <spaceNo> <channel> <time> <outfile>
```

  例えばこんな感じです、  

``` 
python test.py /home/BonDriver/BonDriver_Proxy-T.so 0 72 10 test.ts
```

  視聴テストは、

``` 
python testwatch.py <bondriver> <spaceNo> <channel>
```

  です。gstreamer1.0以上が必要です。

## まだできてないけど、やりたいこと

 * python以外での動作check。

