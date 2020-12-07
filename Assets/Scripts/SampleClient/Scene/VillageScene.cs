﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VillageScene : MonoBehaviour
{
    public GameObject localPlayerPrefab;
    public GameObject otherPlayerPrefab;

    private Actor localPlayer;
    private List<Actor> otherPlayers = new List<Actor>();
    private Dictionary<uint/*serial*/, Actor> actors = new Dictionary<uint/*serial*/, Actor>();

    private void Awake()
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
        Network.Instance.AddBind( Protocol.Both.SyncTransform.PacketType, SyncTransform );
        Network.Instance.AddBind( Protocol.Both.SyncInterpolation.PacketType, SyncInterpolation );
        Network.Instance.AddBind( Protocol.FromServer.CreatePlayer.PacketType, CreatePlayer );
    }

    private void SyncTransform( string _data )
    {
        Protocol.Both.SyncTransform protocol = JsonUtility.FromJson<Protocol.Both.SyncTransform>( _data );

        if ( !actors.ContainsKey( protocol.Player.Serial ) )
        {
            Debug.LogWarning( "actor not Found. Serial = " + protocol.Player.Serial );
            return;
        }

        Actor actor = actors[ protocol.Player.Serial ];
        if ( ReferenceEquals( actor, null ) )
        {
            Debug.LogError( "actor is null. Serial = " + protocol.Player.Serial );
            return;
        }

        actor.rigidBody.position = protocol.Player.Position;
        actor.rigidBody.rotation = protocol.Player.Rotation;
    }

    private void SyncInterpolation( string _data )
    {
        Protocol.Both.SyncInterpolation protocol = JsonUtility.FromJson<Protocol.Both.SyncInterpolation>( _data );

        if ( !actors.ContainsKey( protocol.Player.Serial ) )
        {
            Debug.LogWarning( "actor not Found. Serial = " + protocol.Player.Serial );
            return;
        }

        Actor actor = actors[ protocol.Player.Serial ];
        if ( ReferenceEquals( actor, null ) )
        {
            Debug.LogError( "actor is null. Serial = " + protocol.Player.Serial );
            return;
        }

        actor.rigidBody.MovePosition( protocol.Player.Position );
        actor.rigidBody.MoveRotation( protocol.Player.Rotation );
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

        Actor player = instance.GetComponent<Actor>();
        if ( ReferenceEquals( player, null ) )
        {
            Debug.LogError(  "is not player. Type = " + instance.GetType().Name );
            return;
        }

        player.serial = protocol.Player.Serial;
        player.isLocal = protocol.IsLocal;
        actors.Add( player.serial, player );
        if ( player.isLocal )
        {
            localPlayer = player;
        }
        else
        {
            otherPlayers.Add( player );
        }
    }
}
