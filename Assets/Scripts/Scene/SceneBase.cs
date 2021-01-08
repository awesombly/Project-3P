using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneBase : Singleton<SceneBase>
{
    internal string stageId;

    [SerializeField]
    private GameObject localPlayerPrefab;
    [SerializeField]
    private GameObject otherPlayerPrefab;

    [SerializeField]
    private Transform spawnTransform;

    private Player localPlayer;
    public Player LocalPlayer
    {
        get { return localPlayer; }
        set
        {
            if ( localPlayer != null )
            {
                ObjectManager.Instance.Remove( localPlayer.serial );
                // actors.Remove( localPlayer.serial );
                Destroy( localPlayer.gameObject );
            }

            localPlayer = value;
            OnChangeLocalPlayer?.Invoke( localPlayer );
        }
    }
    public delegate void DelChangeLocalPlayer( Player _localPlayer );
    public event DelChangeLocalPlayer OnChangeLocalPlayer;

    private List<Player> otherPlayers = new List<Player>();

    protected virtual void Awake()
    {
        stageId = gameObject.name;

        Network.Instance.OnConnect += OnConnect;
        Network.Instance.OnBindProtocols += OnBindProtocols;
    }

    protected virtual void Start()
    {
        if ( LocalPlayer == null )
        {
            Player player = FindObjectOfType<Player>();
            if ( player != null && player.isLocal )
            {
                LocalPlayer = player;
            }
        }
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
        protocol.SpawnPosition = spawnTransform.position;
        Network.Instance.Send( protocol );
    }

    protected virtual void OnBindProtocols()
    {
        Network.Instance.AddBind( Protocol.FromServer.Connected.PacketType, Connected );

        Network.Instance.AddBind( Protocol.Both.SyncTransform.PacketType, SyncTransform );
        Network.Instance.AddBind( Protocol.Both.SyncInterpolation.PacketType, SyncInterpolation );
        Network.Instance.AddBind( Protocol.Both.SyncCrouch.PacketType, SyncCrouch );
        Network.Instance.AddBind( Protocol.Both.SyncGrounded.PacketType, SyncGrounded );

        /* Player */
        Network.Instance.AddBind( Protocol.FromServer.CreatePlayer.PacketType, CreatePlayer );
        Network.Instance.AddBind( Protocol.FromServer.DestroyActor.PacketType, DestroyActor );

        /* Npc */
        Network.Instance.AddBind( Protocol.FromServer.RequestNpcInfo.PacketType, RequestNpcInfo );
        Network.Instance.AddBind( Protocol.FromServer.ResponseNpcInfo.PacketType, ResponseNpcinfo );
        Network.Instance.AddBind( Protocol.FromServer.SyncNpcInfo.PacketType, SyncNpcState );
    }

    private void Connected( string _data )
    {
        Network.Instance.InvokeOnConnect();
    }

    private void SyncTransform( string _data )
    {
        Protocol.Both.SyncTransform protocol = JsonUtility.FromJson<Protocol.Both.SyncTransform>( _data );

        Actor actor = ObjectManager.Instance.Find( protocol.Actor.Serial );
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

        Actor actor = ObjectManager.Instance.Find( protocol.Actor.Serial );
        if ( ReferenceEquals( actor, null ) )
        {
            Debug.LogError( "actor is null. Serial = " + protocol.Actor.Serial );
            return;
        }

        actor.rigidBody.MovePosition( protocol.Actor.Position );
        actor.rigidBody.MoveRotation( protocol.Actor.Rotation );
        actor.syncVelocity = protocol.Velocity;
    }

    private void SyncCrouch( string _data )
    {
        Protocol.Both.SyncCrouch protocol = JsonUtility.FromJson<Protocol.Both.SyncCrouch>( _data );

        Player player = ObjectManager.Instance.Find( protocol.Serial ) as Player;
        if ( ReferenceEquals( player, null ) )
        {
            Debug.LogError( "player is null. Serial = " + protocol.Serial );
            return;
        }

        player.IsCrouching = protocol.IsCrouch;
    }

    private void SyncGrounded( string _data )
    {
        Protocol.Both.SyncGrounded protocol = JsonUtility.FromJson<Protocol.Both.SyncGrounded>( _data );

        Player player = ObjectManager.Instance.Find( protocol.Serial ) as Player;
        if ( ReferenceEquals( player, null ) )
        {
            Debug.LogError( "player is null. Serial = " + protocol.Serial );
            return;
        }

        player.IsGrounded = protocol.IsGrounded;
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
        ObjectManager.Instance.Add( player );
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

        Actor actor = ObjectManager.Instance.Find( protocol.Serial );
        otherPlayers.Remove( actor as Player );
        ObjectManager.Instance.Remove( protocol.Serial );

        Destroy( actor.gameObject );
    }

    private void SyncNpcState( string _data )
    {
        Protocol.FromServer.SyncNpcInfo protocol = JsonUtility.FromJson<Protocol.FromServer.SyncNpcInfo>( _data );

        AIBase npc = ObjectManager.Instance.Find( protocol.NpcInfo.Serial ) as AIBase;
        if ( ReferenceEquals( npc, null ) )
        {
            Debug.Log( "Npc not found. Name : " + protocol.NpcInfo.NpcId );
            return;
        }

        npc.Sync( protocol.NpcInfo.Target, protocol.NpcInfo.CurPosition, protocol.NpcInfo.State );
    }

    private void RequestNpcInfo( string _data )
    {
        Protocol.FromServer.RequestNpcInfo requestNpcInfo = JsonUtility.FromJson<Protocol.FromServer.RequestNpcInfo>( _data );

        AIBase npc = ObjectManager.Instance.Find( requestNpcInfo.Serial ) as AIBase;
        if ( ReferenceEquals( npc, null ) )
        {
            Debug.Log( "Npc not found. Serial : " + requestNpcInfo.Serial );
            return;
        }

        Protocol.ToServer.ResponseNpcInfo responseNpcInfo;
        responseNpcInfo.NpcInfo.IsLocal = npc.isLocal;
        responseNpcInfo.NpcInfo.Serial = npc.serial;
        responseNpcInfo.NpcInfo.State = npc.state;
        responseNpcInfo.NpcInfo.NpcId = gameObject.name;
        responseNpcInfo.NpcInfo.Target = npc.target;
        responseNpcInfo.NpcInfo.CurPosition = npc.transform.position;
        
        Network.Instance.Send( responseNpcInfo );
    }

    private void ResponseNpcinfo( string _data )
    {
        Protocol.FromServer.ResponseNpcInfo protocol = JsonUtility.FromJson<Protocol.FromServer.ResponseNpcInfo>( _data );

        if ( !ObjectManager.Instance.Search( protocol.Serial ) )
        {
            AIBase newNpc = GameObject.Find( protocol.NpcId ).GetComponent<AIBase>();
            if ( ReferenceEquals( newNpc, null ) )
            {
                Debug.Log( "Unity hierarchy not found. name : " + protocol.NpcId );
                return;
            }

            newNpc.isLocal = protocol.IsLocal;
            newNpc.serial = protocol.Serial;
            ObjectManager.Instance.Add( newNpc );
        }
    }
}
