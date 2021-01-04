using System.Runtime.InteropServices; // mashal
using UnityEngine;

[StructLayout( LayoutKind.Sequential, Pack = 1 )]
public class UPACKET
{
    public const int HeaderSize = 4;
    public const int DataMaxSize = 2048;

    public ushort length;
    public ushort type;
    // 구조체나 문자열을 전송하고싶은 경우
    // MarshalAs( ... ) 으로 타입과 크기를 명시해 주어야 합니다.
    // 이렇게 해야 컴파일러가 직렬화/역직렬화 할 때 크기를 계산할 수 있습니다.
    [MarshalAs( UnmanagedType.ByValArray, SizeConst = DataMaxSize )]
    public byte[] data;

    public UPACKET( Protocol.IProtocol _protocol )
    {
        SetData( _protocol );
    }

    public void SetData( Protocol.IProtocol _protocol )
    {
        type = _protocol.GetPacketType();
        data = System.Text.Encoding.UTF8.GetBytes( JsonUtility.ToJson( _protocol ) );
        length = ( ushort )( data.Length + HeaderSize );
    }

    // 서버/클라 결과 동일해야함. (Sdbm Hash)
    public static ushort GetPacketType( string _name )
    {
        uint hash = 0;
        foreach ( char elem in _name )
        {
            hash = elem + ( hash << 6 ) + ( hash << 16 ) - hash;
        }
        return ( ushort )hash;
    }
}
