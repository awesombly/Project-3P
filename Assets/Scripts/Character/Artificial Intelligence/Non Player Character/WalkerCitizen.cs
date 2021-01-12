using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkerCitizen : AIBase
{
    [SerializeField]
    private Transform[] spots;

    private readonly WaitForSeconds DefaultWaitTimeCached = new WaitForSeconds( 1.0f );

    protected override void Awake()
    {
        base.Awake();

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();
        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }

        Random.InitState( ( int )( Time.time * Mathf.PI * 10000.0f ) );
        target = spots[ Random.Range( 1, spots.Length ) ].position;

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
            if ( !target.Equals( spots[ value ].position ) )
            {
                target = spots[ value ].position;
                ChangeState( AIState.Move );
            }
            yield return null;
        }
    }

    protected virtual IEnumerator Move()
    {
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

    protected override IEnumerator Interaction()
    {
        nav.avoidancePriority = 40;
        animator.SetInteger( AnimatorParameters.AIState, ( int )AIState.Idle );
        isInteraction = true;
        nav.isStopped = true;

        while ( true )
        {
            yield return null;

            Vector3 dis = target - transform.position;
            Vector3 disXZ = new Vector3( dis.x, 0.0f, dis.z );
            transform.rotation = Quaternion.Lerp( transform.rotation, Quaternion.LookRotation( disXZ ), 0.1f );
        }
    }
}
