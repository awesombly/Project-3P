using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkerCitizen : AIBase
{
    [SerializeField]
    private Transform[] spots;
    [SerializeField]
    private Transform currentSpot;
    
    bool isConnected = false;

    protected override void Awake()
    {
        base.Awake();

        Network.Instance.OnConnect += OnConnect;

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();
        currentSpot = spots[ Random.Range( 1, spots.Length ) ];

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }

        nav.speed = Random.Range( 1.0f, 2.0f );
    }

    protected override IEnumerator Idle()
    {
        Debug.Log( "Current State : Idle" );
        yield return new WaitForSeconds( Random.Range( 1.0f, 2.5f ) );
        while ( true )
        {
            yield return null;

            int index = Random.Range( 1, spots.Length );
            if ( !ReferenceEquals( currentSpot, spots[index] ) )
            {
                currentSpot = spots[index];
                ChangeState( AIState.Move );
            }
        }
    }

    protected virtual IEnumerator Move()
    {
        Debug.Log( "Current State : Move -> " + currentSpot.name );
        nav.isStopped = false;

        while ( true )
        {
            yield return null;
            nav.SetDestination( currentSpot.position );
            if ( !nav.pathPending && nav.remainingDistance <= 2.0f ) 
            {
                ChangeState( AIState.Idle );
                nav.isStopped = true;
            }
        }
    }

    protected void OnConnect()
    {
        isConnected = true;
    }

    private void Update()
    {
        if ( isConnected )
        {
            Protocol.ToServer.RequestNpcInfo protocol;
            protocol.NpcId = gameObject.name;

            protocol.Actor.Serial = 0;
            protocol.Actor.Position = transform.position;
            protocol.Actor.Rotation = transform.rotation;

            Network.Instance.Send( protocol );
            isConnected = false;
        }
        isLocal = false;
    }
}
