using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

using ReceivedPacket = System.Collections.Generic.KeyValuePair<ushort/*packetType*/, string/*data*/>;

public class Network : Singleton<Network>
{
    public string ipAddress = "127.0.0.1";

    private Socket socket;
    private Thread thread;

    private byte[] buffer = new byte[ 1024 * 16 ];
    private Queue<ReceivedPacket> receivedPackets = new Queue<ReceivedPacket>();

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
        if ( ReferenceEquals( packetData, null ) )
        {
            return;
        }

        socket.Send( packetData );
    }

    public void AddBind( ushort _packetType, DelProcessPacket _handler )
    {
        protocols.Add( _packetType, _handler );
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
                catch ( System.ObjectDisposedException ex )
                {
                    Debug.Log( "Connect Ended. Ip = " + ipAddress + ", " + ex.Message );
                    return;
                }
            }

            OnConnect?.Invoke();
        }

        while ( true )
        {
            SocketError error;
            socket.Receive( buffer, 0, buffer.Length, SocketFlags.None, out error );
            if ( error != SocketError.Success )
            {
                Debug.LogError( "Socket receive failed. error = " + error.ToString() );
                return;
            }

            // TODO : 패킷이 중간에 짤린 경우에 대한 처리. (buffer length 이상 들어올시)
            int offset = 0;
            while ( true )
            {
                UPACKET packet = Global.Deserialize<UPACKET>( buffer, offset );
                if ( ReferenceEquals( packet, null ) || packet.length == 0 )
                {
                    break;
                }

                if ( packet.length > buffer.Length )
                {
                    Debug.LogError( "buffer overflow. packet = " + packet.length + ", buffer = "+ buffer.Length );
                    break;
                }
                
                string data = System.Text.Encoding.UTF8.GetString( packet.data, 0, packet.length - UPACKET.HeaderSize );
                receivedPackets.Enqueue( new ReceivedPacket( packet.type, data ) );

                System.Array.Clear( buffer, offset, packet.length );
                offset += packet.length;
            }
        }
    }

    private void Update()
    {
        // 메인 쓰레드 외엔 Instantiate() 같은 작업이 안돼 따로 처리
        while ( receivedPackets.Count > 0 )
        {
            ReceivedPacket packet = receivedPackets.Dequeue();

            if ( !protocols.ContainsKey( packet.Key ) )
            {
                Debug.LogWarning( "Packet not bind. type = " + packet.Key + ", data = " + packet.Value );
                continue;
            }

            DelProcessPacket handler = protocols[ packet.Key ];
            if ( ReferenceEquals( handler, null ) )
            {
                Debug.LogWarning( "Packet handler is null. type = " + packet.Key + ", data = " + packet.Value );
                continue;
            }

            handler.Invoke( packet.Value );
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
    }

    private void ReceiveChatMessage( string _data )
    {
        Protocol.Both.ChatMessage protocol = JsonUtility.FromJson<Protocol.Both.ChatMessage>( _data );

        ChatSystem.Instance.PushMessage( protocol.Message );
        // ChatMain.texts.Add( protocol.Message );
    }
}
