using System.Runtime.InteropServices; // mashal
using UnityEngine;

[StructLayout( LayoutKind.Sequential, Pack = 1 )]
public class UPACKET
{
    public const int HeaderSize = 4;
    public const int DataMaxSize = 2048;

    public ushort length;
    public ushort type;

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
