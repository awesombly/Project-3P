using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VillageScene : MonoBehaviour
{
    public GameObject playerPrefab;

    private GameObject localPlayer;
    private List<GameObject> otherPlayers = new List<GameObject>();

    void Awake()
    {
        Network.Instance.OnConnect += OnConnect;
        Network.Instance.OnBindProtocols += OnBindProtocols;
    }

    private void OnDestroy()
    {
        Network.Instance.OnConnect -= OnConnect;
        Network.Instance.OnBindProtocols -= OnBindProtocols;
    }

    private void OnConnect()
    {
        Protocol.ToServer.EnterStage protocol;
        Network.Instance.Send( protocol );
    }

    private void OnBindProtocols()
    {
        Network.Instance.AddBind( Protocol.FromServer.CreatePlayer.PacketType, CreatePlayer );
    }

    private void CreatePlayer( string _data )
    {
        Protocol.FromServer.CreatePlayer protocol = JsonUtility.FromJson<Protocol.FromServer.CreatePlayer>( _data );

        GameObject player = Instantiate( playerPrefab, protocol.Position, Quaternion.Euler( protocol.Direction ) );
        if ( protocol.IsLocal )
        {
            localPlayer = player;
        }
        else
        {
            otherPlayers.Add( player );
        }
    }
}
