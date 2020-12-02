using System.Runtime.InteropServices; // mashal
using UnityEngine;
using System;
using System.Collections.Generic;

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

namespace Protocol
{
    public interface IProtocol
    {
        ushort GetPacketType();
    }

    public struct ChatMessage : IProtocol
    {
        public string Message;

        public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
        public ushort GetPacketType()
        {
            return PacketType;
        }
    }

    [Serializable]
    public struct TestProtocol : IProtocol
    {
        public int Level;
        public string Id;

        // JsonUtility에서 Dictionary를 지원하지 않는다..
        // keys, values List 쌍으로 흉내낼 수는 있는듯
        //public Dictionary<int/*SlotIndex*/, string/*EquipId*/ > Equipments;

        [Serializable]
        public struct Item
        {
            public int Count;
            public string Id;
        };
        public List<Item> ItemList;

        public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
        public ushort GetPacketType()
        {
            return PacketType;
        }
    }
}