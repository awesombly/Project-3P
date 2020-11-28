using System.Runtime.InteropServices; // mashal
using UnityEngine;

[StructLayout( LayoutKind.Sequential, Pack = 1 )]
public class UPACKET
{
    public static int HeaderSize = 4;
    public ushort length;
    public ushort type;
    // 구조체나 문자열을 전송하고싶은 경우
    // MarshalAs( ... ) 으로 타입과 크기를 명시해 주어야 합니다.
    // 이렇게 해야 컴파일러가 직렬화/역직렬화 할 때 크기를 계산할 수 있습니다.
    [MarshalAs( UnmanagedType.ByValArray, SizeConst = 2048 )]
    public byte[] data;

    public UPACKET( ISerializable target )
    {
        SetData( target );
    }

    public void SetData( ISerializable target )
    {
        type = ( ushort )target.GetHashCode();
        data = System.Text.Encoding.UTF8.GetBytes( JsonUtility.ToJson( target ) );
        length = ( ushort )( data.Length + HeaderSize );
    }
}

public interface ISerializable
{
}

public struct ChatMessage : ISerializable
{
    public string message;

    // 패킷을 구분하기 위한 데이터(임시). 서버랑 맞춰야함
    // 공통 코드. 매크로로 처리되면 좋을듯
    public static ushort HashCode = ( ushort )( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name ).GetHashCode();
    public override int GetHashCode()
    {
        return ( int )HashCode;
    }
}
