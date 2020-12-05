using System.Runtime.InteropServices;

public class Global
{
    public static byte[] Serialize( object _obj )
    {
        int size = Marshal.SizeOf( _obj );
        byte[] data = new byte[size];
        System.IntPtr buffer = Marshal.AllocHGlobal( size + 1 );
        Marshal.StructureToPtr( _obj, buffer, false );
        Marshal.Copy( buffer, data, 0, size );
        Marshal.FreeHGlobal( buffer );

        return data;
    }

    // 역직렬화
    public static Type Deserialize<Type>( byte[] _value )
    {
        int size = Marshal.SizeOf( typeof( Type ) );
        System.IntPtr buffer = Marshal.AllocHGlobal( size );
        Marshal.Copy( _value, 0, buffer, size );
        Type obj = ( Type )Marshal.PtrToStructure( buffer, typeof( Type ) );
        Marshal.FreeHGlobal( buffer );

        return obj;
    }
}