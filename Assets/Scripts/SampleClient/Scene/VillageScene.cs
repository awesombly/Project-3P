using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VillageScene : MonoBehaviour
{
    public GameObject playerPrefab;

    private GameObject localPlayer;
    private List<GameObject> otherPlayers = new List<GameObject>();

    private void Awake()
    {
        Network.Instance.OnConnect += OnConnect;
        Network.Instance.OnBindProtocols += OnBindProtocols;
    }

    private void Update()
    {
        if ( localPlayer != null )
        {
            /// 테스트용.
            Protocol.Both.SyncTransform protocol;
            protocol.Name = localPlayer.name;
            protocol.Position = localPlayer.transform.position;
            protocol.Rotation = localPlayer.transform.rotation;

            Network.Instance.Send( protocol );
        }
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
        Network.Instance.AddBind( Protocol.Both.SyncTransform.PacketType, SyncTransform );
        Network.Instance.AddBind( Protocol.FromServer.CreatePlayer.PacketType, CreatePlayer );
    }

    private void SyncTransform( string _data )
    {
        Protocol.Both.SyncTransform protocol = JsonUtility.FromJson<Protocol.Both.SyncTransform>( _data );

        GameObject player = null;
        if ( localPlayer != null && protocol.Name == localPlayer.name )
        {
            player = localPlayer;
        }
        else
        {
            player = otherPlayers.Find( ( instance ) => { return instance.name == protocol.Name; } );
        }
        
        if ( player == null )
        {
            Debug.LogWarning( "Player not Found. name = " + player.name );
            return;
        }

        player.transform.position = protocol.Position;
        player.transform.rotation = protocol.Rotation;
    }

    private void CreatePlayer( string _data )
    {
        Protocol.FromServer.CreatePlayer protocol = JsonUtility.FromJson<Protocol.FromServer.CreatePlayer>( _data );

        GameObject player = Instantiate( playerPrefab, protocol.Position, Quaternion.Euler( protocol.Direction ) );
        player.name = protocol.Name;

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
