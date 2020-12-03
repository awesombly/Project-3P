using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class Network : Singleton<Network>
{
    public string ipAddress = "127.0.0.1";

    private Socket socket;
    private Thread thread;

    private byte[] buffer = new byte[ UPACKET.DataMaxSize + UPACKET.HeaderSize ];

    public delegate void DelProcessPacket( string _data );
    private Dictionary<ushort/*packetType*/, DelProcessPacket> protocols = new Dictionary<ushort/*packetType*/, DelProcessPacket>();

    public delegate void DelConnect();
    public event DelConnect OnConnect;

    public delegate void DelBindProtocols();
    public event DelBindProtocols OnBindProtocols;

    public void Send( Protocol.IProtocol _protocol )
    {
        if ( ReferenceEquals( socket, null ) || !socket.Connected )
        {
            Debug.LogError( "Not Connected." );
            return;
        }

        UPACKET packet = new UPACKET( _protocol );

        byte[] packetData = Global.Serialize( packet );
        socket.Send( packetData );
    }

    public void AddBind( ushort _packetType, DelProcessPacket _processor )
    {
        protocols.Add( _packetType, _processor );
    }

    private void Run()
    {
        // Connecting
        {
            socket = new Socket( AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp );
            IPEndPoint endPoint = new IPEndPoint( IPAddress.Parse( ipAddress ), 10000 );

            while ( !socket.Connected )
            {
                try
                {
                    socket.Connect( endPoint );
                }
                catch ( SocketException ex )
                {
                    Debug.LogError( "Connect Failed. Ip = " + ipAddress + ", " + ex.Message );
                }
            }

            OnConnect?.Invoke();
        }

        while ( true )
        {
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

    private void Awake()
    {
        OnConnect += Connected;
        OnBindProtocols += BindProtocols;
    }

    private void Start()
    {
        OnBindProtocols?.Invoke();

        thread = new Thread( Run );
        thread.Start();
    }

    private void OnDestroy()
    {
        socket.Close();
    }

    private void Connected()
    {
        Debug.Log( "Connected. Ip = " + ipAddress );
    }

    private void BindProtocols()
    {
        AddBind( Protocol.Both.ChatMessage.PacketType, ReceiveChatMessage );
        AddBind( Protocol.Both.TestProtocol.PacketType, ReceiveTestProtocol );
    }

    private void ReceiveChatMessage( string _data )
    {
        Protocol.Both.ChatMessage protocol = JsonUtility.FromJson<Protocol.Both.ChatMessage>( _data );

        ChatMain.texts.Add( protocol.Message );
    }

    private void ReceiveTestProtocol( string _data )
    {
        Debug.Log( _data );
        Protocol.Both.TestProtocol protocol = JsonUtility.FromJson<Protocol.Both.TestProtocol>( _data );
    }
}
