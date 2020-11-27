using System.Collections;
using System.Collections.Generic;
using System.Net; // socket
using System.Net.Sockets; // socket
using System.Runtime.InteropServices; // mashal
using System.Threading; // thread
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
            socket.Receive( buffer );
            if ( !ReferenceEquals( buffer, null ) )
            {
                UPACKET packet = Global.Deserialize<UPACKET>( buffer );
                switch ( packet.type )
                {
                    case ( short )1000:
                        {
                            ChatMain.texts.Add( System.Text.Encoding.UTF8.GetString( packet.message ) );
                        }
                        break;

                    default:
                        {

                        }
                        break;
                }
                System.Array.Clear( buffer, 0, packet.length );
            }
        }
    }

    private void Start()
    {
        // TCP IP 소켓 생성
        socket = new Socket( AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp );

        // 서버 연결, 콜백주소 사용
        IPEndPoint endPoint = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), 10000 );
        socket.Connect( endPoint );

        thread = new Thread( Run );
        thread.Start();
    }

    private void OnDestroy()
    {
        // 테스트용으로 객체가 사라질때 닫기로 함
        // 소켓 닫기
        socket.Close();
    }
}