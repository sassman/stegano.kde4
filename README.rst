Stegano.kde4 a Steganographic tool for KDE4
===========================================

still under development - beta status

the .NET based Windows Version can be found here_

.. _here: http://svenomenal.net/devel/steganoV2

What does this tool do
----------------------

You can wrap text messages (and also documents) into pictures. The so called steganography enables that. Some more details about steganography you can find on wikipedia_.

The method behind is the LSB (Last Significant Bit) that means an image can be manipulate lets say by 1 bit, without any affect to a user that looks at this picture. This is also true for other media like audio and video (mp3 is a perfect sample for this).
For instance if you take one pixel, that have lets say 3 byte for color information (1 for each color), you can modify each color (RGB) at least by 1 bit without any change of the picture from a optical perspective. This means you can store 3 bit of information in 1 pixel. For one letter (based on UTF-8 - 2 bytes) you need around 6 pixel to store it. The good thing is that reallity pictures like from a camera have a lot of pixel, so normally you can store a lot of data (also documents) in it.

To make the whole solution bit more robust, i added encryption as an optional feature. Therefore you can make sure that the stored information is only accessibly by ones that have a valid password.

The only low hanging foot is that currently only PNG as save format is supported. The reason for this that PNG not uses any compression. The small issue with compression or scaling an image with steganographic data is that the information will be lost afterwards.

.. _wikipedia: http://en.wikipedia.org/wiki/Steganography

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

