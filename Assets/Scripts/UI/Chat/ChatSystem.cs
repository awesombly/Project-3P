using System.Text;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public struct Message
{
    public TextMeshProUGUI textObject;
}

public class ChatSystem : Singleton<ChatSystem>
{
    public event ChatReturnEvent ChatEvent;
    public delegate void ChatReturnEvent( bool _isActive );

    private List<Message> messages = new List<Message>();

    private InputField enterContent;
    private GameObject contents;
    private GameObject textPrefab;

    private readonly int maxMessageCount = 25;
    private StringBuilder text = new StringBuilder( 50 );

    private Coroutine fadeOutCoroutine = null;
    private readonly WaitForSeconds fadeOutWaitSeconds = new WaitForSeconds( 5.0f );
    private float fadeOutDuration = 0.5f;

    public void PushMessage( string _msg )
    {
        MakeMessage( _msg );
        if ( !ReferenceEquals( fadeOutCoroutine, null ) )
        {
            StopCoroutine( fadeOutCoroutine );
        }
        ResetTextAlpha();
        fadeOutCoroutine = StartCoroutine( FadeOutContents( fadeOutDuration ) );
    }

    private void MakeMessage( string _text )
    {
        if ( messages.Count >= maxMessageCount )
        {
            Destroy( messages[0].textObject.gameObject );
            messages.Remove( messages[0] );
        }

        Message newMessage = new Message();
        GameObject newText = Instantiate( textPrefab, contents.transform );
        newMessage.textObject = newText.GetComponent<TextMeshProUGUI>();

        text.Length = 0;
        text.Append( "<color=white>" ).Append( System.DateTime.Now.ToString( "HH:mm " ) ).Append( "</color>" );
        text.Append( "<#ffa4a4>" ).Append( "Name " ).Append( "</color>" );
        text.Append( "<#9ddaff>" ).Append( _text ).Append( "</color>" );

        newMessage.textObject.text = text.ToString();
            //"<color=white>" + System.DateTime.Now.ToString( "HH:mm " ) + "</color>" +
            //"<#ffa4a4>" + "Name " + "</color>" +
            //"<#9ddaff>" + newMessage.text + "</color>";

        messages.Add( newMessage );
    }

    private void Start()
    {
        textPrefab = Resources.Load( "UI/Chat/Prefabs/CopyText" ) as GameObject;
        if ( ReferenceEquals( textPrefab, null ) )
        {
            Debug.Log( "TextPrefab Load Failed" );
        }

        enterContent = transform.Find( "EnterContent" ).GetComponent<InputField>();
        if ( ReferenceEquals( enterContent, null ) )
        {
            Debug.Log( "EnterContent Load Failed" );
        }

        Transform[] transforms = gameObject.GetComponentsInChildren<Transform>();
        foreach( Transform child in transforms )
        {
            if ( child.name.Equals( "Contents" ) )
            {
                contents = child.gameObject;
                break;
            }
        }

        if ( ReferenceEquals( contents, null ) )
        {
            Debug.Log( "Contents Load Failed" );
        }

        StartCoroutine( Process() );
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

    private void ResetTextAlpha()
    {
        foreach ( Message msg in messages )
        {
            msg.textObject.CrossFadeAlpha( 1.0f, 0.0f, false );
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

    private IEnumerator Process()
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
                    fadeOutCoroutine = StartCoroutine( FadeOutContents( fadeOutDuration ) );
                }
                else
                {
                    if ( !ReferenceEquals( fadeOutCoroutine, null ) )
                    {
                        StopCoroutine( fadeOutCoroutine );
                    }
                    ResetTextAlpha();

                    enterContent.gameObject.SetActive( true );
                    enterContent.ActivateInputField();
                }

                ChatEvent( enterContent.gameObject.activeSelf );
            }
        }
    }
}
