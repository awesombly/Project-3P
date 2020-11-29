using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class Network : MonoBehaviour
{
    public static Socket socket;
    private Thread thread;

    private byte[] buffer = new byte[ UPACKET.DataMaxSize + UPACKET.HeaderSize ];

    public delegate void DelProcessPacket( string data );
    private Dictionary<ushort/*packetType*/, DelProcessPacket> protocols = new Dictionary<ushort/*packetType*/, DelProcessPacket>();

    private void Run()
    {
        while ( true )
        {
            if ( !socket.Connected )
            {
                continue;
            }

            socket.Receive( buffer );
            if ( !ReferenceEquals( buffer, null ) )
            {
                UPACKET packet = Global.Deserialize<UPACKET>( buffer );
                if ( protocols.ContainsKey( packet.type ) )
                {
                    string data = System.Text.Encoding.UTF8.GetString( packet.data, 0, packet.length - UPACKET.HeaderSize );
                    protocols[ packet.type ]?.Invoke( data );
                }

                System.Array.Clear( buffer, 0, packet.length );
            }
        }
    }

    private void Start()
    {
        BindProtocols();

        socket = new Socket( AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp );

        IPEndPoint endPoint = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), 10000 );
        socket.ConnectAsync( endPoint );

        thread = new Thread( Run );
        thread.Start();
    }

    private void OnDestroy()
    {
        // 테스트용으로 객체가 사라질때 닫기로 함
        socket.Close();
    }

    private void BindProtocols()
    {
        protocols.Add( Protocol.ChatMessage.PacketType, ReceiveChatMessage );
    }

    private void ReceiveChatMessage( string data )
    {
        Protocol.ChatMessage protocol = JsonUtility.FromJson<Protocol.ChatMessage>( data );

        ChatMain.texts.Add( protocol.Message );
    }
}
