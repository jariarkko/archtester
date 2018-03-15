
# Network Architecture Tester

The Network Architecture Tester (NAT) is a tool for testing whether a given network has a clean architecture. The software is designed to run on a Raspberry Pi environment, but is easily portable to other environments as well.

The GitHub home for this project is https://github.com/jariarkko/archtester. The tool has been written by Jari Arkko at Ericsson Research. It also includes, separately, the hopping and tlsver tools, see https://github.com/jariarkko/hopping and https://github.com/jariarkko/tlsver.

The basic operation of the tester is to plug it into an Ethernet cable, and let the tester determine whether the network is correctly designed. While the tester is establishing a connection to the network, the yellow led blinks. If the network is correctly designed, a green light lights up. Otherwise, the red "Architecture Fault" light lights up.

The tester architecture is configurable and network-driven; the device where the tester runs has a framework that supports user-defined testing methodology. One user may employ a basic connectivity test while another one may wish to check for path purity.

![Image of a tester](https://github.com/jariarkko/archtester/blob/master/doc/tester.jpg?raw=true)

# Installation

The easiest installation method is to retrieve the software from GitHub. There is one dependency on the hopping program, which needs to fetched and compiled first. These commands should suffice:

    git clone https://github.com/jariarkko/hopping.git
    cd hopping
    sudo make all install
    cd ..
    
    git clone https://github.com/jariarkko/tlsver.git
    cd tlsver
    sudo make all install
    cd ..
    
    git clone https://github.com/jariarkko/archtester.git
    cd archtester
    sudo make all install
  
# Projects

Further development is still needed. We'd like to see for instance:

* Building the user-pluggable interface; for now, the operation of the tester is completely fixed.
* Adding a web-based UI where you can control the operation of the tester, or see more details. The idea is that each tester would have its own web page where you can see the most current measurement, be able to configure wireless LAN parameters, etc.
* Adding support for wifi-networks.
* The addition of a "selector" switch that can be used to select the type of an architectural measurement, e.g., direct connectivity, path purity, number of hops, and so on.
* The development of further tests that facilitate the measurement of architectural factors on a given network or on the Internet.