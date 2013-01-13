Stegano.kde4 a Steganographic tool for KDE4
===========================================

still under development - pre beta status

the .NET based Windows Version can be found here_

.. _here: http://svenomenal.net/devel/steganoV2

Build Dependencies
==================

- Qt Crypto library
- QuaZIP_ library

.. _QuaZIP:  http://quazip.sourceforge.net/

::

    $ sudo apt-get install libqca2-dev qca2-utils libquazip0-dev libquazip0


Build it
========

::

    git clone https://github.com/sassman/stegano.kde4.git stegano.kde4 && cd stegano.kde4
    mkdir build && cd build && cmake ../
    make