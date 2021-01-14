﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectManager : Singleton<ObjectManager>
{
    private Player localPlayer;
    public Player LocalPlayer
    {
        get { return localPlayer; }
        set
        {
            if ( localPlayer != null )
            {
                Remove( localPlayer );
                Destroy( localPlayer.gameObject );
            }

            localPlayer = value;
            OnChangeLocalPlayer?.Invoke( localPlayer );
        }
    }
    public delegate void DelChangeLocalPlayer( Player _localPlayer );
    public event DelChangeLocalPlayer OnChangeLocalPlayer;

    private readonly Dictionary<uint /* serial */, Actor> actors = new Dictionary<uint, Actor>();

    private readonly Dictionary<uint /* serial */, Actor> players = new Dictionary<uint, Actor>();
    public Dictionary<uint, Actor>.ValueCollection Players
    {
        get
        {
            return players.Values;
        }
    }

    private readonly Dictionary<uint /* serial */, Actor> npcs = new Dictionary<uint, Actor>();
    public Dictionary<uint, Actor>.ValueCollection Npcs
    {
        get
        {
            return npcs.Values;
        }
    }

    private void Awake()
    {
        SceneBase.OnBeforeSceneLoad += OnBeforeSceneLoad;
    }

    public void Add( Actor _actor )
    {
        if ( actors.ContainsKey( _actor.serial ) ||
             ReferenceEquals( _actor, null ) )
        {
            Debug.Log( "ObjectManager::Push Serial overlab or parameter is null." );
            return;
        }

        actors.Add( _actor.serial, _actor );

        if ( _actor.CompareTag( "Npc" ) )
        {
            npcs.Add( _actor.serial, _actor );
        }
        else if ( _actor.CompareTag( "Player" ) )
        {
            players.Add( _actor.serial, _actor );
            if ( _actor.isLocal )
            {
                LocalPlayer = _actor as Player;
            }
        }
    }

    public void Remove( Actor _actor )
    {
        if ( !actors.ContainsKey( _actor.serial ) ||
             ReferenceEquals( _actor, null ) )
        {
            Debug.Log( "ObjectManager::Erase Serial not found or parameter is null." );
            return;
        }

        actors.Remove( _actor.serial );

        if ( _actor.CompareTag( "Npc" ) )
        {
            npcs.Remove( _actor.serial );
        }
        else if ( _actor.CompareTag( "Player" ) )
        {
            players.Remove( _actor.serial );
        }
    }

    public void Remove( uint _serial )
    {
        if ( !actors.ContainsKey( _serial ) )
        {
            Debug.Log( "ObjectManager::Erase Serial not found." );
            return;
        }

        actors.Remove( _serial );
    }

    public Actor Find( uint _serial )
    {
        if ( !actors.ContainsKey( _serial ) )
        {
            Debug.Log( "ObjectManager::Erase Serial not found." );
            return null;
        }

        return actors[ _serial ];
    }

    public bool Search( uint _serial )
    {
        return actors.ContainsKey( _serial );
    }

    public void ClearAll()
    {
        actors.Clear();
        players.Clear();
        npcs.Clear();
        LocalPlayer = null;
    }

    private void OnBeforeSceneLoad()
    {
        ClearAll();
    }
}
