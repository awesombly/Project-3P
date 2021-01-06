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
                actors.Remove( localPlayer.serial );
                Destroy( localPlayer.gameObject );
            }

            localPlayer = value;
            OnChangeLocalPlayer?.Invoke( localPlayer );
        }
    }
    public delegate void DelChangeLocalPlayer( Player _localPlayer );
    public event DelChangeLocalPlayer OnChangeLocalPlayer;

    private List<Player> otherPlayers = new List<Player>();
    private Dictionary<uint/*serial*/, Actor> actors = new Dictionary<uint/*serial*/, Actor>();
    private Dictionary<string /* npc name */, AIBase> npcs = new Dictionary<string, AIBase>();

    public Actor GetActor( uint _serial )
    {
        if ( !actors.ContainsKey( _serial ) )
        {
            Debug.LogWarning( "actor not Found. serial = " + _serial );
            return null;
        }

        return actors[ _serial ];
    }

    public AIBase GetNpc( string _name )
    {
        if ( !npcs.ContainsKey( _name ) )
        {
            Debug.LogWarning( "npc not Found. name = " + _name );
            return null;
        }

        return npcs[_name];
    }

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
        Network.Instance.AddBind( Protocol.Both.SyncNpcState.PacketType, SyncNpcState );

        Network.Instance.AddBind( Protocol.FromServer.CreatePlayer.PacketType, CreatePlayer );
        Network.Instance.AddBind( Protocol.FromServer.DestroyActor.PacketType, DestroyActor );
        Network.Instance.AddBind( Protocol.FromServer.ResponseNpcInfo.PacketType, ResponseNpcInfo );
    }

    private void Connected( string _data )
    {
        Network.Instance.InvokeOnConnect();
    }

    private void SyncNpcState( string _data )
    {
        Protocol.Both.SyncNpcState protocol = JsonUtility.FromJson<Protocol.Both.SyncNpcState>( _data );
        
        AIBase npc = GetNpc( protocol.NpcInfo.NpcId );
        if ( ReferenceEquals( npc, null ) )
        {
            Debug.LogError( "npc is null. name = " + protocol.NpcInfo.NpcId );
            return;
        }

        npc.SyncState( ( AIBase.AIState )protocol.NpcInfo.State, protocol.NpcInfo.Target, protocol.NpcInfo.CurPosition );
    }

    private void SyncTransform( string _data )
    {
        Protocol.Both.SyncTransform protocol = JsonUtility.FromJson<Protocol.Both.SyncTransform>( _data );

        Actor actor = GetActor( protocol.Actor.Serial );
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

        Actor actor = GetActor( protocol.Actor.Serial );
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

        Player player = GetActor( protocol.Serial ) as Player;
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

        Player player = GetActor( protocol.Serial ) as Player;
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

        Actor actor = GetActor( protocol.Serial );
        otherPlayers.Remove( actor as Player );
        actors.Remove( protocol.Serial );

        Destroy( actor.gameObject );
    }

    private void ResponseNpcInfo( string _data )
    {
        Protocol.FromServer.ResponseNpcInfo protocol = JsonUtility.FromJson<Protocol.FromServer.ResponseNpcInfo>( _data );

        GameObject obj = GameObject.Find( protocol.NpcInfo.NpcId );
        obj.transform.position = protocol.NpcInfo.CurPosition;

        npcs.Add( obj.name, obj.GetComponent<AIBase>() );
    }
}
