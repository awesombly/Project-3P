using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;

// 솔루션 탐색기에 UnityCppServerConsole이 안보이면
// 기존 프로젝트 추가 -> Server폴더 찾으셔서 추가하세요.

public struct Message
{
    public string text;
    public Text textObject;
}

public class ChatMain : MonoBehaviour
{
    public List<Message> messages = new List<Message>();
    public static List<string> texts = new List<string>();

    public InputField inputMessage;
    public GameObject textContents;
    public GameObject textObject;

    private const int maxMessageCount = 25;

    private void Update()
    {
        if ( texts.Count > 0 )
        {
            MakeMessage( texts[0] );
        }
    }

    public void MakeMessage( string _text )
    {
        if ( messages.Count >= maxMessageCount )
        {
            Destroy( messages[0].textObject.gameObject );
            messages.Remove( messages[0] );
        }

        Message newMessage = new Message();
        newMessage.text = _text;

        GameObject newText = Instantiate( textObject, textContents.transform );
        newMessage.textObject = newText.GetComponent<Text>();
        newMessage.textObject.text = newMessage.text;
        messages.Add( newMessage );

        texts.Remove( _text );
    }

    // 채팅바( InputField )에 입력한 내용이 서버로 전송됩니다.
    public void SendChatMessage()
    {
        // Input Field 내용을 UTF-8 형식으로 인코딩 합니다.
        // 영어나 숫자는 1바이트으로 보내도 큰 문제가 없지만,
        // 한글을 보낼시에 글자가 깨져서 보내지기 때문에 
        // 인코딩 형식을 UTF-8형식으로 통일했습니다.
        byte[] msg = System.Text.Encoding.UTF8.GetBytes( inputMessage.text );

        UPACKET packet = new UPACKET();
        // 서버 패킷의 message( data ) 기본 크기를 2048바이트로 잡았기 때문에 통일 합니다.
        packet.message = new byte[2048];
        // 메세지 바이트 사이즈 + 헤더 바이트 사이즈( 아직 Const 작업 안함 )
        packet.length = ( short )( msg.Length + 4 );
        // 패킷 타입 : 메세지( 1000 ) 
        // 아직 Define 작업 안함.
        packet.type = 1000;

        for ( int count = 0; count < msg.Length; count++ )
        {
            packet.message[count] = msg[count];
        }

        byte[] _packet = Global.Serialize( packet );
        Network.socket.Send( _packet );
        inputMessage.text = "";
    }
}
