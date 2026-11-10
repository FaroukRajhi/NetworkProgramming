import java.io.*;
import java.net.*;


public class DownloadWebPage
{
    public static void main(String[] args)
    {
        try
        {
            URL u = new URL("https://www.google.com");
            URLConnection uc = u.openConnection();


            try (InputStream raw = uc.getInputStream()) {
                InputStream buffer = new BufferedInputStream(raw);
                Reader reader = new InputStreamReader(buffer);

                int c;
                while ((c = reader.read()) != -1) {
                    System.out.print((char) c);
                }
            }  catch (MalformedURLException ex) {
                  System.err.println("The URL is not in a parseable format.");
        }
        } catch (IOException ex) {
            System.err.println("Could not read from the connection: " + ex);
    }
}