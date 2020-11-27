using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

[StructLayout( LayoutKind.Sequential )]
public class UPACKET
{
    public short length;
    public short type;
    // 구조체나 문자열을 전송하고싶은 경우
    // MarshalAs( ... ) 으로 타입과 크기를 명시해 주어야 합니다.
    // 이렇게 해야 컴파일러가 직렬화/역직렬화 할 때 크기를 계산할 수 있습니다.
    [MarshalAs( UnmanagedType.ByValArray, SizeConst = 2048 )]
    public byte[] message;
}