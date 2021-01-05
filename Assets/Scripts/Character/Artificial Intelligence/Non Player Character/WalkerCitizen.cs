using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkerCitizen : AIBase
{
    [SerializeField]
    private Transform[] spots;
    [SerializeField]
    private Transform currentSpot;

    protected override void Awake()
    {
        base.Awake();

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();
        currentSpot = spots[4];//Random.Range( 1, spots.Length ) ];

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }

        nav.speed = 1.0f;
    }

    protected override IEnumerator Idle()
    {
        Debug.Log( "Current State : Idle" );
        yield return new WaitForSeconds( 2.5f );
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
}
