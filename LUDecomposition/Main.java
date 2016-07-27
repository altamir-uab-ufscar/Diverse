//import java.awt.*;
import java.io.*;
import java.math.*;
import java.text.*;
import java.util.*;

public class Main {
  public static void main(String[] args) {
    InputStream inputStream = System.in;
    OutputStream outputStream = System.out;
    InputReader in = new InputReader(inputStream);
    PrintWriter out = new PrintWriter(outputStream);
    LUDecomposition solver = new LUDecomposition();
    solver.solve(1, in, out);
    out.close();
  }
  
  static class LUDecomposition {    
    void solve(int testNumber, InputReader in, PrintWriter out) {
      int n = in.nextInt();
      double[][] mat = new double[n][n];
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          mat[i][j] = in.nextDouble();
        }
      }
      double[][][] lu = crout(mat);
      if (lu != null) {
        for (double[][] i : lu) {
          for (double[] j : i) {
            out.println(Arrays.toString(j));
          }
          out.println();
        }
      } else {
        out.println("LU factorization failed.");
      }
      double[][] prod = matMul(lu[0], lu[1]);
      if (prod != null) {
        for (double[] i : prod) {
          out.println(Arrays.toString(i));
        }
      }
    }
    
    double[][][] crout(double[][] a) {
      int n = a.length;
      double[][][] lu = new double[2][n][n];
      for (int k = 0; k < n; ++k) {
        lu[1][k][k] = 1.;
      }
      for (int k = 0; k < n; ++k) {
        for (int j = k; j < n; ++j) {
          double acc = 0.;
          for (int m = 0; m < k; ++m) {
            acc += lu[0][j][m] * lu[1][m][k];
          }
          lu[0][j][k] = a[j][k] - acc;
        }
        for (int i = k; i < n; ++i) {
          double sum = 0.;
          for (int m = 0; m < k; ++m) {
            sum += lu[0][k][m] * lu[1][m][i];
          }
          if (lu[0][k][k] == 0.) {
            return null;
          }
          lu[1][k][i] = (a[k][i] - sum) / lu[0][k][k];
        }
      }
      return lu;
    }
    
    double[][] matMul(double[][] a, double[][] b) {
      if (a[0].length != b.length) {
        return null;
      }
      int m = a.length, n = b[0].length, p = a[0].length;
      double[][] c = new double[m][n];
      for (int k = 0; k < p; ++k) {
        for (int i = 0; i < m; ++i) {
          for (int j = 0; j < n; ++j) {
            c[i][j] += a[i][k] * b[k][j];
          }
        }
      }
      return c;
    }
    
  }
      
  static class InputReader {
    public BufferedReader reader;
    public StringTokenizer tokenizer;

    public InputReader(InputStream stream) {
      reader = new BufferedReader(new InputStreamReader(stream));
      tokenizer = null;
    }

    public String next() {
      while (tokenizer == null || !tokenizer.hasMoreTokens()) {
        try {
          tokenizer = new StringTokenizer(reader.readLine());
        } catch (IOException e) {
          throw new RuntimeException(e);
        }
      }
      return tokenizer.nextToken();
    }
    
    public String nextLine() {
      try {
        return reader.readLine();
      } catch(IOException e) {
        throw new RuntimeException(e);
      }
    }

    public int nextInt() {
      return Integer.parseInt(next());
    }

    public long nextLong() {
      return Long.parseLong(next());
    }
    
    public double nextDouble() {
      return Double.parseDouble(next());
    }

    public boolean hasInput() {
      try {
        if (tokenizer != null && tokenizer.hasMoreTokens()) {
          return true;
        }
        reader.mark(1);
        int ch = reader.read();
        if (ch != -1) {
          reader.reset();
          return true;
        }
        return false;
      } catch(IOException e) {
        throw new RuntimeException(e);
      }
    }
    
  }
}
