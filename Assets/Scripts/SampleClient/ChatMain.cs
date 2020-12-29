using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

// 솔루션 탐색기에 UnityCppServerConsole이 안보이면
// 기존 프로젝트 추가 -> Server폴더 찾으셔서 추가하세요.

public struct Message
{
    public string text;
    public Text textObject;
}

public class ChatMain : MonoBehaviour
{
    public static List<string> texts = new List<string>();
    public List<Message> messages = new List<Message>();

    public InputField inputMessage;
    public GameObject chatPannel;
    public GameObject textContent;
    public GameObject textPrefab;

    private const int maxMessageCount = 25;

    private void Update()
    {
        if ( Input.GetKeyDown( KeyCode.Return ) )
        {
            OnVisibleChatPannel( true );
        }

        if ( texts.Count > 0 )
        {
            MakeMessage( texts[ 0 ] );
        }

        
    }

    public void MakeMessage( string _text )
    {
        if ( messages.Count >= maxMessageCount )
        {
            Destroy( messages[ 0 ].textObject.gameObject );
            messages.Remove( messages[ 0 ] );
        }

        Message newMessage = new Message();
        newMessage.text = _text;

        GameObject newText = Instantiate( textPrefab, textContent.transform );
        newMessage.textObject = newText.GetComponent<Text>();
        newMessage.textObject.text = newMessage.text;
        messages.Add( newMessage );

        texts.Remove( _text );
    }

    public void SendChatMessage()
    {
        Protocol.Both.ChatMessage protocol;
        protocol.Message = inputMessage.text;
        
        Network.Instance.Send( protocol );
        inputMessage.text = "";
    }

    public void OnVisibleChatPannel( bool isVisible )
    {
        if ( chatPannel != null )
        {
            chatPannel.SetActive( isVisible );
        }

        if ( isVisible )
        {
            inputMessage.Select();
        }
    }
}
