package gov.llnl.casc.rose;

public interface GenericVector<T> {
  public long size();
    // public long capacity();
    // public void reserve(long n);
  public boolean isEmpty();
  public void clear();
  public void add(T x);
  public T get(int i);
    // public void set(int i, T x);
}
