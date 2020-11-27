using System.Runtime.InteropServices;

public class Global
{
    // 직렬화는 데이터 구조나 오브젝트 상태를 동일하거나 
    // 다른 컴퓨터 환경에서 저장하고 나중에 재구성 할 수 있는 포맷으로 변환하는 
    // 과정 입니다.

    // 마샬링 : 오브젝트를 직렬화하는 과정

    // [Serializable](직렬화 가능한 구조체)가 붙은 구조체 또는
    // 원시타입( int, long, float ... )만 직렬화 할 수 있습니다.

    // 직렬화
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