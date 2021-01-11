using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectManager : Singleton<ObjectManager>
{
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
}
