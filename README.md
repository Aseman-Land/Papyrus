### Papyrus

Papyrus is a kaqaz base note manager, focused on privacy and socials features.
Papyrus project are released under the terms of the GPLv3 license.

### How to Compile
#### Install dependencies

Install gcc, g++, openssl, git, Qt5Core, Qt5DBus, Qt5Gui, Qt5Multimedia, Qt5MultimediaQuick_p, Qt5Network, Qt5PrintSupport, Qt5Qml, Qt5Quick, Qt5Sql, Qt5Svg, and Qt5Widgets.
on Ubuntu:

    sudo apt-get install g++ gcc git qtbase5-dev libqt5sql5-sqlite libqt5multimediaquick-p5 libqt5multimedia5-plugins libqt5multimedia5 libqt5qml5 libqt5qml-graphicaleffects libqt5qml-quickcontrols qtdeclarative5-dev libqt5quick5 

on Fedora (tested on Fedora 20):

    yum install qt5-qtbase qt5-qtbase-devel qt5-qtdeclarative qt5-qtquick1 qt5-qtquick1-devel kde-plasma-applicationname kde-plasma-nm qt5-qtdeclarative-devel qt5-qtdeclarative-static qt5-qtgraphicaleffects qt5-qtquickcontrols openssl-devel

For other distributions search for the corresponding packages.

#### Get source code from git repository

Get Papyrus code using this command:

    git clone https://github.com/Aseman-Land/Papyrus.git

#### Start building

Switch to source directory

    cd Papyrus

##### Ubuntu

    mkdir build && cd build
    qmake -r .. PREFIX=/usr
    make
    make install
    
for ubuntu touch add ubuntutouch to BUILD_MODE flag:

    qmake -r .. PREFIX=/usr BUILD_MODE+=ubuntutouch

You can use command below after building to clean build directory on the each step.

    make clean

##### Fedora

    mkdir build && cd build
    /bin/qmake-qt5 -o Makefile ..
    make

#### Run Papyrus
    
To run papyrus touch mode run below command:

    papyrus --touch
    
and normal mode:

    papyrus
