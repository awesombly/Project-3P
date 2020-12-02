using System;
using System.Collections.Generic;


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
