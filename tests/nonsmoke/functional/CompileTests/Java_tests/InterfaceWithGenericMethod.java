import java.io.InputStream;
import java.io.OutputStream;
import javax.security.auth.callback.Callback;
interface InterfaceWithGenericMethod {
    public <T extends InputStream>void register(String name, String desc, T input);
    public <T extends OutputStream>void register(String name, String desc, T output);
    public void register(Callback callback);
}
