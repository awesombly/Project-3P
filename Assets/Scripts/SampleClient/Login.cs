using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Login : MonoBehaviour
{
    public InputField idFeild;
    public InputField pwFeild;

    public void ReqLoginCheck()
    {
        //Protocol.LoginReq protocol;
        //protocol.ID = idFeild.text;
        //protocol.PW = pwFeild.text;

        //Network.Instance.Send( protocol );
    }

    private void AckLoginCheck( string _data )
    {
        Debug.Log( _data );
    }

    private void Awake()
    {
        //Network.Instance.protocols.Add( Protocol.LoginAck.PacketType, AckLoginCheck );
    }
}
