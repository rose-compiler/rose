import java.io.*;
import java.util.*;

public class T8_NestMembersAttribute {
    InputReader in;
    PrintWriter out;
    
    public void run() {
        in = new InputReader(System.in);
        out = new PrintWriter(System.out);
        out.close();
    }

    class InputReader {
        BufferedReader br;
        StringTokenizer st;

        InputReader(File f) {
            try {
                br = new BufferedReader(new FileReader(f));
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
        }

        public InputReader(InputStream f) {
            br = new BufferedReader(new InputStreamReader(f));
        }

        String next() {
            while (st == null || !st.hasMoreTokens()) {
                try {
                    st = new StringTokenizer(br.readLine());
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            return st.nextToken();
        }

        int nextInt() {
            return Integer.parseInt(next());
        }
    }

    public static void main(String[] arg) {
        new T8_NestMembersAttribute().run();
    }
}
