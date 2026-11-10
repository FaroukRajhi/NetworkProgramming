/*
Inspecting UDP packets.
Prints socket contents.
*/

import java.io.*;
import java.net.*;

public class Datagram
{
    public static void main(String[] args)
    {

        String test = "Just a test message";

        try
        {
            byte[] data = test.getBytes("UTF-8");
            InetAddress ia = InetAddress.getByName("www.google.com");
            int port = 7;

            DatagramPacket dp = new DatagramPacket(data,data.length, ia, port);

            System.out.println("This packet is addressed to " + dp.getAddress() + " on port: " + dp.getPort());
            System.out.println("There are " + dp.getLength() + " bytes of data in the packet");
            System.out.println(new String(dp.getData(), dp.getOffset(), dp.getLength(), "UTF-8"));
        } catch (UnknownHostException | UnsupportedEncodingException ex) {
            System.err.println(ex);
        }
    }
}