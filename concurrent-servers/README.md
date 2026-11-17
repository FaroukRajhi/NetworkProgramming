The protocol is stateful. the server changes internal state based on data client send.

1. wait for client.
2. Client connected (Input), send request (its output).
3. Server waits for client message.
4. Server receives message (client input: receives char x [x != $]).
5. Server response output to client (char x+1).
6. the server ignores everything the client sends until it see a ^ character that signals that a new message begins.
7. When the client send a $, the server goes back to waiting for a new message.

=> The ^ and $ characters are only used to delimit messages.