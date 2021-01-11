using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;
using UnityEngine.ResourceManagement.AsyncOperations;

public class SceneBase : MonoBehaviour
{
    internal string stageId;

    [SerializeField]
    private GameObject localPlayerPrefab;
    [SerializeField]
    private GameObject otherPlayerPrefab;

    [SerializeField]
    private Transform spawnTransform;

    public delegate void DelChangeScene();
    public static event DelChangeScene OnChangeScene;

    protected virtual void Awake()
    {
        stageId = gameObject.name;

        Network.Instance.OnConnect += OnConnect;
        Network.Instance.OnBindProtocols += OnBindProtocols;
    }

    protected virtual void Start()
    {
    }

    protected virtual void OnDestroy()
    {
        Network.Instance.OnConnect -= OnConnect;
        Network.Instance.OnBindProtocols -= OnBindProtocols;
    }

    public virtual void ChangeSceneAsync( AssetReference _scene )
    {
        Addressables.LoadSceneAsync( _scene ).Completed += ( _handle ) =>
        {
            if ( _handle.Status != AsyncOperationStatus.Succeeded )
            {
                Debug.LogError( "Failed LoadScene. scene = " + _scene );
                return;
            }

            OnChangeScene?.Invoke();
        };
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

        /* Scene */
        Network.Instance.AddBind( Protocol.FromServer.CreatePlayer.PacketType, CreatePlayer );
        Network.Instance.AddBind( Protocol.FromServer.DestroyActor.PacketType, DestroyActor );

        /* Player */
        Network.Instance.AddBind( Protocol.Both.SyncTransform.PacketType, SyncTransform );
        Network.Instance.AddBind( Protocol.Both.SyncInterpolation.PacketType, SyncInterpolation );
        Network.Instance.AddBind( Protocol.Both.SyncCrouch.PacketType, SyncCrouch );
        Network.Instance.AddBind( Protocol.Both.SyncGrounded.PacketType, SyncGrounded );
        Network.Instance.AddBind( Protocol.Both.SyncEquipment.PacketType, SyncEquipment );

        /* Npc */
        Network.Instance.AddBind( Protocol.Both.SyncNpcInteraction.PacketType, SyncNpcInteraction );
        Network.Instance.AddBind( Protocol.FromServer.SyncNpcInfo.PacketType, SyncNpcInfo );
        Network.Instance.AddBind( Protocol.FromServer.ChangedStageHost.PacketType, ChangedStageHost );
        Network.Instance.AddBind( Protocol.FromServer.RequestHostNpcInfo.PacketType, RequestHostNpcInfo );
        Network.Instance.AddBind( Protocol.FromServer.ResponseNpcInfo.PacketType, ResponseNpcinfo );
    }

    #region Protocols
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

    private void SyncEquipment( string _data )
    {
        Protocol.Both.SyncEquipment protocol = JsonUtility.FromJson<Protocol.Both.SyncEquipment>( _data );

        Player player = ObjectManager.Instance.Find( protocol.Serial ) as Player;
        if ( ReferenceEquals( player, null ) )
        {
            Debug.LogError( "player is null. Serial = " + protocol.Serial );
            return;
        }

        Equipment equip = ResourceManager.Instance.GetAsset<Equipment>( protocol.Guid );
        if ( ReferenceEquals( equip, null ) )
        {
            Debug.LogError( "Equipment not found. Guid = " + protocol.Guid );
            return;
        }

        player.SetEquipment( equip );
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
    }

    private void DestroyActor( string _data )
    {
        Protocol.FromServer.DestroyActor protocol = JsonUtility.FromJson<Protocol.FromServer.DestroyActor>( _data );

        Actor actor = ObjectManager.Instance.Find( protocol.Serial );
        ObjectManager.Instance.Remove( protocol.Serial );

        Destroy( actor.gameObject );
    }

    private void SyncNpcInfo( string _data )
    {
        Protocol.FromServer.SyncNpcInfo protocol = JsonUtility.FromJson<Protocol.FromServer.SyncNpcInfo>( _data );

        AIBase npc = ObjectManager.Instance.Find( protocol.NpcInfo.Actor.Serial ) as AIBase;
        if ( ReferenceEquals( npc, null ) )
        {
            Debug.Log( "Npc not found. Name : " + protocol.NpcInfo.NpcId );
            return;
        }

        npc.Sync( protocol.NpcInfo.Target, protocol.NpcInfo.Actor.Position, protocol.NpcInfo.State );
    }

    private void RequestHostNpcInfo( string _data )
    {
        Protocol.FromServer.RequestHostNpcInfo requestNpcInfo = JsonUtility.FromJson<Protocol.FromServer.RequestHostNpcInfo>( _data );

        AIBase npc = ObjectManager.Instance.Find( requestNpcInfo.Serial ) as AIBase;
        if ( ReferenceEquals( npc, null ) )
        {
            Debug.Log( "Npc not found. Serial : " + requestNpcInfo.Serial );
            return;
        }

        Protocol.ToServer.ResponseHostNpcInfo protocol;
        protocol.NpcInfo.IsLocal = npc.isLocal;
        protocol.NpcInfo.Actor.Serial = npc.serial;
        protocol.NpcInfo.Actor.Position = npc.transform.position;
        protocol.NpcInfo.Actor.Rotation = npc.transform.rotation;
        protocol.NpcInfo.State = npc.state;
        protocol.NpcInfo.NpcId = npc.gameObject.name;
        protocol.NpcInfo.Target = npc.target;

        Network.Instance.Send( protocol );
    }

    private void ResponseNpcinfo( string _data )
    {
        Protocol.FromServer.ResponseNpcInfo protocol = JsonUtility.FromJson<Protocol.FromServer.ResponseNpcInfo>( _data );

        if ( !ObjectManager.Instance.Search( protocol.Serial ) )
        {
            AIBase newNpc = GameObject.Find( protocol.NpcId ).GetComponent<AIBase>();
            if ( ReferenceEquals( newNpc, null ) )
            {
                Debug.Log( "Could not find npc in Unity hierarchy. name : " + protocol.NpcId );
                return;
            }

            newNpc.isLocal = protocol.IsLocal;
            newNpc.serial = protocol.Serial;
            ObjectManager.Instance.Add( newNpc );
        }

        if ( !protocol.IsLocal )
        {
            Protocol.ToServer.RequestNpcSync requestNpcSync;
            requestNpcSync.Serial = protocol.Serial;

            Network.Instance.Send( requestNpcSync );
        }
    }

    private void ChangedStageHost( string _data )
    {
        foreach( AIBase npc in ObjectManager.Instance.Npcs )
        {
            npc.isLocal = true;
        }
    }

    private void SyncNpcInteraction( string _data )
    {
        Protocol.Both.SyncNpcInteraction protocol = JsonUtility.FromJson<Protocol.Both.SyncNpcInteraction>( _data );
        
        AIBase npc = ObjectManager.Instance.Find( protocol.NpcSerial ) as AIBase;
        if ( ReferenceEquals( npc, null ) )
        {
            Debug.Log( "npc not found. npc is null. serial : " + protocol.NpcSerial );
            return;
        }

        Player player = ObjectManager.Instance.Find( protocol.PlayerSerial ) as Player;
        if ( Global.FakeNullCheck( player ) )
        {
            Debug.Log( "Player not found. player is null. serial : " + protocol.PlayerSerial );
            return;
        }

        npc.SyncInteration( player );
    }
    #endregion
}
