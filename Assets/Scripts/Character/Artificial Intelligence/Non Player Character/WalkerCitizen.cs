using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkerCitizen : AIBase
{
    [SerializeField]
    private Transform[] spots;

    protected override void Awake()
    {
        base.Awake();

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();
        Random.InitState( ( int )Time.time );
        target = spots[ Random.Range( 1, spots.Length ) ].position;

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }
    }

    protected override IEnumerator Idle()
    {
        Random.InitState( ( int )( Time.time * Mathf.PI * 10000.0f ) );
        while ( true )
        {
            if ( !isLocal )
            {
                yield return null;
            }
            
            int value = Random.Range( 1, spots.Length );
            if ( !target.Equals( spots[value].position ) )
            {
                target = spots[value].position;
                ChangeState( AIState.Move );
            }
            yield return null;
        }
    }

    protected virtual IEnumerator Move()
    {
        nav.isStopped = false;

        while ( true )
        {
            yield return null;
            nav.SetDestination( target );
            if ( !nav.pathPending && nav.remainingDistance <= 2.0f ) 
            {
                nav.isStopped = true;
                if ( isLocal )
                {
                    ChangeState( AIState.Idle );
                }
            }
        }
    }
}
