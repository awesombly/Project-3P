using UnityEngine;
using System;
using System.Collections.Generic;

// 서버/클라 통신용
[Serializable]
public struct ServerActor
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

        public struct SyncTransform : IProtocol
        {
            public ServerActor Actor;

            public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
            public ushort GetPacketType()
            {
                return PacketType;
            }
        }

        public struct SyncInterpolation : IProtocol
        {
            public ServerActor Actor;
            public Vector3 Velocity;

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
            public string StageId;
            public Vector3 SpawnPosition;

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
            public ServerActor Player;
            public bool IsLocal;

            public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
            public ushort GetPacketType()
            {
                return PacketType;
            }
        }

        public struct DestroyActor : IProtocol
        {
            public uint Serial;

            public static ushort PacketType = UPACKET.GetPacketType( System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name );
            public ushort GetPacketType()
            {
                return PacketType;
            }
        }
    }
}
