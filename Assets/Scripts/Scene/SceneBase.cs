using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneBase : Singleton<SceneBase>
{
    [SerializeField]
    private GameObject localPlayerPrefab;
    [SerializeField]
    private GameObject otherPlayerPrefab;

    [SerializeField]
    private Transform spawnTransform;
    private Vector3 spawnPosition;

    private Player localPlayer;
    public Player LocalPlayer
    {
        get { return localPlayer; }
        set 
        {
            localPlayer = value;
            OnChangeLocalPlayer?.Invoke( localPlayer );
        }
    }
    public delegate void DelChangeLocalPlayer( Player localPlayer );
    public event DelChangeLocalPlayer OnChangeLocalPlayer;

    private List<Actor> otherPlayers = new List<Actor>();
    private Dictionary<uint/*serial*/, Actor> actors = new Dictionary<uint/*serial*/, Actor>();

    internal string stageId;

    protected virtual void Awake()
    {
        // position 접근이 메인 쓰레드에서만 돼서 임시조치
        spawnPosition = spawnTransform.position;
        stageId = gameObject.name;

        Network.Instance.OnConnect += OnConnect;
        Network.Instance.OnBindProtocols += OnBindProtocols;
    }

    protected virtual void OnDestroy()
    {
        Network.Instance.OnConnect -= OnConnect;
        Network.Instance.OnBindProtocols -= OnBindProtocols;
    }

    protected virtual void OnConnect()
    {
        Protocol.ToServer.EnterStage protocol;
        protocol.StageId = stageId;
        protocol.SpawnPosition = spawnPosition;
        Network.Instance.Send( protocol );
    }

    protected virtual void OnBindProtocols()
    {
        Network.Instance.AddBind( Protocol.Both.SyncTransform.PacketType, SyncTransform );
        Network.Instance.AddBind( Protocol.Both.SyncInterpolation.PacketType, SyncInterpolation );
        Network.Instance.AddBind( Protocol.FromServer.CreatePlayer.PacketType, CreatePlayer );
        Network.Instance.AddBind( Protocol.FromServer.DestroyActor.PacketType, DestroyActor );
    }

    private void SyncTransform( string _data )
    {
        Protocol.Both.SyncTransform protocol = JsonUtility.FromJson<Protocol.Both.SyncTransform>( _data );

        if ( !actors.ContainsKey( protocol.Actor.Serial ) )
        {
            Debug.LogWarning( "actor not Found. Serial = " + protocol.Actor.Serial );
            return;
        }

        Actor actor = actors[ protocol.Actor.Serial ];
        if ( ReferenceEquals( actor, null ) )
        {
            Debug.LogError( "actor is null. Serial = " + protocol.Actor.Serial );
            return;
        }

        actor.rigidBody.position = protocol.Actor.Position;
        actor.rigidBody.rotation = protocol.Actor.Rotation;
    }

    private void SyncInterpolation( string _data )
    {
        Protocol.Both.SyncInterpolation protocol = JsonUtility.FromJson<Protocol.Both.SyncInterpolation>( _data );

        if ( !actors.ContainsKey( protocol.Actor.Serial ) )
        {
            Debug.LogWarning( "actor not Found. Serial = " + protocol.Actor.Serial );
            return;
        }

        Actor actor = actors[ protocol.Actor.Serial ];
        if ( ReferenceEquals( actor, null ) )
        {
            Debug.LogError( "actor is null. Serial = " + protocol.Actor.Serial );
            return;
        }

        actor.rigidBody.MovePosition( protocol.Actor.Position );
        actor.rigidBody.MoveRotation( protocol.Actor.Rotation );
        actor.syncVelocity = protocol.Velocity;
    }

    private void CreatePlayer( string _data )
    {
        Protocol.FromServer.CreatePlayer protocol = JsonUtility.FromJson<Protocol.FromServer.CreatePlayer>( _data );
        GameObject instance = null;

        if ( protocol.IsLocal )
        {
            instance = Instantiate( localPlayerPrefab, protocol.Player.Position, protocol.Player.Rotation );
        }
        else
        {
            instance = Instantiate( otherPlayerPrefab, protocol.Player.Position, protocol.Player.Rotation );
        }

        if ( instance == null )
        {
            Debug.LogError( "instance is null. : " + protocol.ToString() );
            return;
        }

        Player player = instance.GetComponent<Player>();
        if ( ReferenceEquals( player, null ) )
        {
            Debug.LogError( "is not player. Type = " + instance.GetType().Name );
            return;
        }

        player.serial = protocol.Player.Serial;
        player.isLocal = protocol.IsLocal;
        actors.Add( player.serial, player );
        if ( player.isLocal )
        {
            LocalPlayer = player;
        }
        else
        {
            otherPlayers.Add( player );
        }
    }

    private void DestroyActor( string _data )
    {
        Protocol.FromServer.DestroyActor protocol = JsonUtility.FromJson<Protocol.FromServer.DestroyActor>( _data );
        if ( !actors.ContainsKey( protocol.Serial ) )
        {
            Debug.LogError( "Actor not found. Serial = " + protocol.Serial );
            return;
        }

        Actor actor = actors[ protocol.Serial ];
        otherPlayers.Remove( actor );
        actors.Remove( protocol.Serial );

        Destroy( actor.gameObject );
    }
}
