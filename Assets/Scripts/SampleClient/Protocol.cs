using UnityEngine;
using System;
using System.Collections.Generic;

// 서버/클라 통신용
[Serializable]
public struct ServerObject
{
    public uint Serial;
    public Vector3 Position;
    public Quaternion Rotation;
}

namespace Protocol
{
    public interface IProtocol
    {
        ushort GetPacketType();
    }

    // Both : 클라/서버 양쪽에서 사용
    // ToServer : 서버로 보내는 패킷
    // FromServer : 서버에서 온 패킷
    namespace Both
    {
        public struct ChatMessage : IProtocol
        {
            public string Message;

            public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
            public ushort GetPacketType()
            {
                return PacketType;
            }
        }

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

        public struct SyncTransform : IProtocol
        {
            public ServerObject Player;

            public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
            public ushort GetPacketType()
            {
                return PacketType;
            }
        }
    }
    
    namespace ToServer
    {
        public struct EnterStage : IProtocol
        {
            public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
            public ushort GetPacketType()
            {
                return PacketType;
            }
        }
    }

    namespace FromServer
    {
        public struct CreatePlayer : IProtocol
        {
            public ServerObject Player;
            public bool IsLocal;

            public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
            public ushort GetPacketType()
            {
                return PacketType;
            }
        }
    }
}
