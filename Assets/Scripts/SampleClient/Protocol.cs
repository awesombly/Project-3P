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

    public UPACKET( ISerializable protocol )
    {
        SetData( protocol );
    }

    public void SetData( ISerializable protocol )
    {
        type = ( ushort )protocol.GetHashCode();
        data = System.Text.Encoding.UTF8.GetBytes( JsonUtility.ToJson( protocol ) );
        length = ( ushort )( data.Length + HeaderSize );
    }
}

public interface ISerializable
{
}

public struct ChatMessage : ISerializable
{
    public string Message;

    // 패킷을 구분하기 위한 데이터(임시). 서버랑 맞춰야함
    // 공통 코드. 매크로로 처리되면 좋을듯
    public static ushort HashCode = ( ushort )( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name ).GetHashCode();
    public override int GetHashCode()
    {
        return ( int )HashCode;
    }
}
