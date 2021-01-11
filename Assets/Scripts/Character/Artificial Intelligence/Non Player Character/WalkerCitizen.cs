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

        Random.InitState( ( int )( Time.time * Mathf.PI * 10000.0f ) );
        target = spots[ Random.Range( 1, spots.Length ) ].position;

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }
    }

    protected override IEnumerator Interaction()
    {
        animator.SetInteger( AnimatorParameters.AIState, ( int )AIState.Idle );
        isInteraction = true;

        while ( true )
        {
            yield return null;

            if ( !isLocal && !nav.pathPending && nav.remainingDistance >= 1.0f )
            {
                nav.SetDestination( target );

                continue;
            }

            nav.isStopped = true;
            Vector3 dis =  target - transform.position;
            Vector3 disXZ = new Vector3( dis.x, 0.0f, dis.z );
            transform.rotation = Quaternion.Lerp( transform.rotation, Quaternion.LookRotation( disXZ ), 0.1f );
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
