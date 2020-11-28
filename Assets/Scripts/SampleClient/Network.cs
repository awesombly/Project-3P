using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class Network : MonoBehaviour
{
    public static Socket socket;
    private Thread thread;

    private byte[] buffer = new byte[2048 + 4];

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
                if ( packet.type == ChatMessage.HashCode )
                {
                    string data = System.Text.Encoding.UTF8.GetString( packet.data );
                    ChatMessage protocol = JsonUtility.FromJson<ChatMessage>( data );
                    ChatMain.texts.Add( protocol.message );
                }

                System.Array.Clear( buffer, 0, packet.length );
            }
        }
    }

    private void Start()
    {
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
}