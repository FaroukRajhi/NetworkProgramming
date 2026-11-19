# Blocking vs. nonblocking I/O

Blocking I/O used to I/O APIs working. While receiving data from a socket,
a call to recv blocks until some data is received from the peer connected to the other side of the socket.
As long as one thread is blocked on I/O, other threads can continue using the CPU.
The other threads are put to sleep by the OS and only wakes up when whatever it was waiting for is available.


NonBlocking I/O : When a socket is set to nonblocking mode, a call to recv (and to send) will always return very quickly, even if there's no data to receive.