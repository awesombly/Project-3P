using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VillageScene : MonoBehaviour
{
    public GameObject localPlayerPrefab;
    public GameObject otherPlayerPrefab;

    private GameObject localPlayer;
    private List<GameObject> otherPlayers = new List<GameObject>();

    [SerializeField]
    private float syncDelay = 0.5f;
    private float curSyncDelay = 0.0f;

    private void Awake()
    {
        Network.Instance.OnConnect += OnConnect;
        Network.Instance.OnBindProtocols += OnBindProtocols;
    }

    private void Update()
    {
        curSyncDelay += Time.deltaTime;
        if ( curSyncDelay >= syncDelay && localPlayer != null )
        {
            curSyncDelay = 0.0f;
            /// 테스트용.
            Protocol.Both.SyncTransform protocol;
            protocol.Player.Name = localPlayer.name;
            protocol.Player.Position = localPlayer.transform.position;
            protocol.Player.Rotation = localPlayer.transform.rotation;

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
        if ( localPlayer != null && protocol.Player.Name == localPlayer.name )
        {
            player = localPlayer;
        }
        else
        {
            player = otherPlayers.Find( ( instance ) => { return instance.name == protocol.Player.Name; } );
        }
        
        if ( player == null )
        {
            Debug.LogWarning( "Player not Found. name = " + protocol.Player.Name );
            return;
        }

        player.transform.position = protocol.Player.Position;
        player.transform.rotation = protocol.Player.Rotation;
    }

    private void CreatePlayer( string _data )
    {
        Protocol.FromServer.CreatePlayer protocol = JsonUtility.FromJson<Protocol.FromServer.CreatePlayer>( _data );
        GameObject player = null;
        
        if ( protocol.IsLocal )
        {
            localPlayer = player = Instantiate( localPlayerPrefab, protocol.Player.Position, protocol.Player.Rotation );
        }
        else
        {
            player = Instantiate( otherPlayerPrefab, protocol.Player.Position, protocol.Player.Rotation );
            otherPlayers.Add( player );
        }

        if ( player == null )
        {
            Debug.LogError( "CreatePlayer failed : " + protocol.ToString() );
            return;
        }

        player.name = protocol.Player.Name;
    }
}
