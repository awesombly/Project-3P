using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public struct Message
{
    public string text;
    public TextMeshProUGUI textObject;
}

public class ChatMain : MonoBehaviour
{
    public static List<string> texts = new List<string>();
    public List<Message> messages = new List<Message>();

    public InputField enterContent;
    public GameObject contents;
    public GameObject textPrefab;

    public delegate void ChatReturnEvent();
    public static event ChatReturnEvent ChatEvent;

    private const int maxMessageCount = 25;
    
    private void Awake()
    {
        enterContent.gameObject.SetActive( false );
    }

    private void Update()
    {
        if ( Input.GetKeyDown( KeyCode.Return )  )
        {
            if ( enterContent.IsActive() )
            {
                SendChatMessage( enterContent.text );
                enterContent.text = "";

                enterContent.gameObject.SetActive( false );
            }
            else
            {
                enterContent.gameObject.SetActive( true );
                enterContent.ActivateInputField();
            }

            ChatEvent();
        }

        if ( texts.Count > 0 )
        {
            MakeMessage( texts[ 0 ] );
        }
    }

    private void SendChatMessage( string _message )
    {
        if ( !_message.Equals( "" ) )
        {
            Protocol.Both.ChatMessage protocol;
            protocol.Message = _message;

            Network.Instance.Send( protocol );
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

        GameObject newText = Instantiate( textPrefab, contents.transform );
        newMessage.textObject = newText.GetComponent<TextMeshProUGUI>();
        newMessage.textObject.text = 
            "<color=white>" + System.DateTime.Now.ToString( "HH:mm " ) + "</color>" +
            "<#ffa4a4>" + "Name " + "</color>" +
            "<#9ddaff>" + newMessage.text + "</color>";

        messages.Add( newMessage );

        texts.Remove( _text );
    }
}
