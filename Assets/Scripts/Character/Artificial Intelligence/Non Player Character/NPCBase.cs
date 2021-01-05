using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class NPCBase : AIBase
{
    [SerializeField]
    // private List<Transform> spots = new List<Transform>();
    private Transform[] spots;
    [SerializeField]
    private Transform currentSpot;

    private NavMeshAgent nav;

    protected override void Awake()
    {
        base.Awake();

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();
        currentSpot = spots[ Random.Range( 1, spots.Length - 1 ) ];
        // spots.AddRange( GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>() );
        // spots.RemoveAt( 0 );

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }

        nav = GetComponent<NavMeshAgent>();
        nav.speed = 10.0f;
    }

    protected override IEnumerator Idle()
    {
        Debug.Log( "Current State : Idle" );
        while ( true )
        {
            yield return null;

            int index = Random.Range( 1, spots.Length );
            if ( !ReferenceEquals( currentSpot, spots[index] ) )
            {
                currentSpot = spots[index];
                Debug.Log( "Next Spot :" + currentSpot.name );
                ChangeState( AIState.Move );
            }
        }
    }

    protected virtual IEnumerator Move()
    {
        Debug.Log( "Current State : Move" );
        while ( true )
        {
            yield return null;
            nav.destination = currentSpot.position;
            if ( !nav.pathPending && nav.remainingDistance <= 2.0f ) 
            {
                ChangeState( AIState.Idle );
            }

        }
    }
}
