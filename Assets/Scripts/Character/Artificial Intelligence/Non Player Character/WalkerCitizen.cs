using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkerCitizen : AIBase
{
    [SerializeField]
    private Transform[] spots;
    private readonly WaitForSeconds waitForSecondsCached = new WaitForSeconds( 1.0f );

    protected override void Awake()
    {
        base.Awake();

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();
        target = spots[ Random.Range( 1, spots.Length ) ].position;

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }
    }

    protected override IEnumerator Idle()
    {
        Debug.Log( "Current State : Idle" );
        while ( true )
        {
            yield return null;

            int index = Random.Range( 1, spots.Length );
            if ( isLocal && !ReferenceEquals( target, spots[index] ) )
            {
                target = spots[index].position;

                yield return waitForSecondsCached;
                ChangeState( AIState.Move );
            }
        }
    }

    protected virtual IEnumerator Move()
    {
        Debug.Log( "Current State : Move : " + target );
        nav.isStopped = false;

        while ( true )
        {
            yield return null;
            nav.SetDestination( target );
            if ( isLocal && !nav.pathPending && nav.remainingDistance <= 2.0f ) 
            {
                nav.isStopped = true;
                ChangeState( AIState.Idle );
            }
        }
    }
}
