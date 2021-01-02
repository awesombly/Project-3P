using System.Collections;
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
    
    public static event ChatReturnEvent ChatEvent;
    public delegate void ChatReturnEvent();

    public InputField enterContent;
    public GameObject contents;
    public GameObject textPrefab;

    private const int maxMessageCount = 25;

    private WaitForSeconds waitFadeOutTime = new WaitForSeconds( 2.0f );
    private float fadeOutDuration = 2.0f;

    private void Awake()
    {
        StartCoroutine( "InputStart" );
        enterContent.gameObject.SetActive( false );
    }

    private void Update()
    {
        if ( texts.Count > 0 )
        {
            MakeMessage( texts[0] );
        }
    }

    private IEnumerator InputStart()
    {
        while ( true )
        {
            yield return null;
            if ( Input.GetKeyDown( KeyCode.Return ) )
            {
                if ( enterContent.IsActive() )
                {
                    SendChatMessage( enterContent.text );
                    enterContent.text = "";

                    enterContent.gameObject.SetActive( false );
                    StartCoroutine( "ContentsFadeOut" );
                }
                else
                {
                    StopCoroutine( "ContentsFadeOut" );
                    foreach ( Message msg in messages )
                    {
                        msg.textObject.CrossFadeAlpha( 1.0f, 0.0f, false );
                    }

                    enterContent.gameObject.SetActive( true );
                    enterContent.ActivateInputField();
                }

                ChatEvent();
            }
        }
    }

    private IEnumerator ContentsFadeOut()
    {
        yield return waitFadeOutTime;

        foreach ( Message msg in messages )
        {
            msg.textObject.CrossFadeAlpha( 0.0f, fadeOutDuration, false );
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
