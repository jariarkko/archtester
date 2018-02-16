
# Network Architecture Tester

The Network Architecture Tester (NAT) is a tool for testing whether a given network has a clean architecture. The software is designed to run on a Raspberry Pi environment, but is easily portable to other environments as well.

The basic operation of the tester is to plug it into an Ethernet cable, and let the tester determine whether the network is correctly designed. While the tester is establishing a connection to the network, the yellow led blinks. If the network is correctly designed, a green light lights up. Otherwise, the red "Architecture Fault" light lights up.

The tester architecture is configurable and network-driven; the device where the tester runs has a framework that supports user-defined testing methodology. One user may employ a basic connectivity test while another one may wish to check for path purity.

![Image of a tester](https://github.com/jariarkko/archtester/blob/master/doc/tester.jpg?raw=true)

# Installation

The easiest installation method is to retrieve the software from GitHub:

  git clone https://github.com/jariarkko/archtester.git



