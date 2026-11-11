The TCP/IP stack is a set of communication protocols that allows computers, servers, or any nodes to communicate over a network or the internet.

Send request and receive a content.
TCP/IP makes this communication possible.

**TCP/IP Stack**

Layers:

4th layer: Application -> Provides network services to apps
3th layer: Transport -> Manages communication between apps
2nd layer: Internet -> Handles IP addressing and routing
1st layer: Network access -> Handles transmission over the physical network. (Deals with Ethernet cables. Wi-Fi, MAC addresse, NIC, Ethernet Frames)

**Linux TUN/TAP**
We will build a program that communicates directly with the linux kernel through a TAP network interface.

Linux provides a virtual network interface called TUN/TAP.
It allows a userspace program to receive and transmit network data through a virtual network interface. 

- TUN (Layer 3): Works with IP packets.
The app receives IP packets directly, without Ethernet headers.
E.g IPv4 packets.
- TAP (Layer 2): Works with Ethernet frames. Ethernet, ARP, IPv4, IPv6.

![alt text](image-1.png)

Check whether the TUN/TAP devices exists:

```
ls -lh /dev/net/tun 
```
