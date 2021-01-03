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

public class ChatSystem : Singleton<ChatSystem>
{
    public List<Message> messages = new List<Message>();
    
    public event ChatReturnEvent ChatEvent;
    public delegate void ChatReturnEvent();

    public InputField enterContent;
    public GameObject contents;
    public GameObject textPrefab;

    private const int maxMessageCount = 25;

    private readonly WaitForSeconds fadeOutWaitSeconds = new WaitForSeconds( 5.0f );
    private float fadeOutDuration = 0.5f;

    public void PushMessage( string _msg )
    {
        MakeMessage( _msg );
        StopCoroutine( "FadeOutContents" );
        FadeInContents( 0.0f );
        StartCoroutine( FadeOutContents( fadeOutDuration ) );
    }

    private void MakeMessage( string _text )
    {
        if ( messages.Count >= maxMessageCount )
        {
            Destroy( messages[0].textObject.gameObject );
            messages.Remove( messages[0] );
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
    }

    private void Awake()
    {
        StartCoroutine( "InputStart" );
        StartCoroutine( "ProcessTexts" );
        enterContent.gameObject.SetActive( false );
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

    private void FadeInContents( float _duration )
    {
        foreach ( Message msg in messages )
        {
            msg.textObject.CrossFadeAlpha( 1.0f, _duration, false );
        }
    }

    private IEnumerator FadeOutContents( float _duration )
    {
        yield return fadeOutWaitSeconds;

        foreach ( Message msg in messages )
        {
            msg.textObject.CrossFadeAlpha( 0.0f, _duration, false );
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
                    StartCoroutine( FadeOutContents( fadeOutDuration ) );
                }
                else
                {
                    StopCoroutine( "FadeOutContents" );
                    FadeInContents( 0.0f );

                    enterContent.gameObject.SetActive( true );
                    enterContent.ActivateInputField();
                }

                ChatEvent();
            }
        }
    }
}
