Stegano.kde4 a Steganographic tool for KDE4
===========================================

still under development - pre beta status

the .NET based Windows Version can be found here_

.. _here: http://svenomenal.net/devel/steganoV2

Build Dependencies
------------------

- Qt Crypto library
- QuaZIP_ library

.. _QuaZIP:  http://quazip.sourceforge.net/

install on kubuntu e.g.

::

    sudo apt-get install libqca2-dev qca2-utils libquazip0-dev libquazip0


Build it
--------

build it into your system (not my prefered way)

::

    git clone https://github.com/sassman/stegano.kde4.git stegano.kde4 && cd stegano.kde4
    mkdir build && cd build && cmake ../
    make && make install

one other way is to use your home dir for kde specific storage. The CMAKE_INSTALL_PREFIX variable needs to point
to a directory of your choice. I suggest the following

:: 

    cd build
    mkdir $HOME/playground
    cmake -DCMAKE_INSTALL_PREFIX=$HOME/playground ../
    make && make install

now under $HOME/playground you will find all kde specific files that makes it really easy to cleanup the system without messing up your installation.

