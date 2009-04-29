#!/bin/sh

CPPFLAGS=-DUSBINARY ./configure --prefix=/usr \
	--disable-shared --without-conf --sysconfdir=/etc \
     --with-vcl \
#     --enable-debug --enable-profile \
	$*

echo ""
echo ""
echo ""
echo "Configured to NOT write a global /etc/sword.conf on 'make install'."
echo "If this is the first time you've installed sword, be sure to run"
echo "'make install_config' if you would like a basic configuration installed"
echo ""
echo "Next you might try something like: "
echo ""
echo "make"
echo "su"
echo "make install"
echo "(and optionally)"
echo "make install_config"
echo "make register"
echo ""
echo ""
echo ""