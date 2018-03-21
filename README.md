
# Network Architecture Tester

The Network Architecture Tester (NAT) is a tool for testing whether a given network has a clean architecture. The software is designed to run on a Raspberry Pi environment, but is easily portable to other environments as well.

The GitHub home for this project is https://github.com/jariarkko/archtester. The tool has been written by Jari Arkko at Ericsson Research and is open source under the BSD license. It also includes, separately, the hopping and tlsver tools, see https://github.com/jariarkko/hopping and https://github.com/jariarkko/tlsver.

The basic operation of the tester is to plug it into an Ethernet cable, and let the tester determine whether the network is correctly designed. While the tester is establishing a connection to the network, the yellow led blinks. If the network is correctly designed, a green light lights up. Otherwise, the red "Architecture Fault" light lights up.

The tester architecture is configurable and network-driven; the device where the tester runs has a framework that supports user-defined testing methodology. One user may employ a basic connectivity test while another one may wish to check for path purity.

![Image of a tester](https://github.com/jariarkko/archtester/blob/master/doc/tester.jpg?raw=true)

# Usage

The device should be plugged into USB power (requires micro-USB at 2A). The device comes up in about a minute, and lights its leds in a specific sequence. Once the yellow led is blinking, the tester is ready for testing. Plug the device into a given network using an Ethernet cable, and in a few seconds the test results should be seen in the leds. If the yellow led continues to blink, the given network does not have apporpriate connectictivity for the tests to be performed.

The devices have no user interface beyond the leds. Internally, they have an identifier (dev0001 etc), and, when network connectivity is available, the device fetches configuration information on desired measurement mode etc from the network. Otherwise, previously used mode is applied. Software updates are checked when the device boots and is connected to a wired network.

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

# Currently supported measurements

The following measurements are currently supported:

- addressing
- domain names
- TLS version
- hops

![Image of a tester](https://github.com/jariarkko/archtester/blob/master/doc/testercloseup.jpg?raw=true)

These measurements are discussed in more detailed below:

## Addressing

The tester lights a green light if the device gets either a public IPv4 address or has global IPv6 address. With no connectivity, the device blinks the yellow light. With just a NATted address and no IPv6, the device lights a red light.

## Domain names

The tester lights a green light if the number of global TLDs is within sensible limits (defined to be under 2000).

## TLS version

The tester lights a green light if among the top 10 most popular web sites, there is at least some that employ TLS 1.3. This behaviour changes on 1.1.2018, when the device will require at least half of the sites to employ LTS 1.3. TLS versions beyond 1.3 or 2.0 will always light a green light.

## Hops

The tester lights a green light if the number of hops "to the center of the Internet" (defined as the website of popular content provider such as Google). If the number of hops is 11 or less, the tester lights the green light.

# Projects

Further development is still needed. We'd like to see for instance:

* Adding a web-based UI where you can control the operation of the tester, or see more details. The idea is that each tester would have its own web page where you can see the most current measurement, be able to configure wireless LAN parameters, etc.
* Adding support for wifi-networks.
* The development of further tests that facilitate the measurement of architectural factors on a given network or on the Internet.
* Adding connectivity confirmation to the addressing test
