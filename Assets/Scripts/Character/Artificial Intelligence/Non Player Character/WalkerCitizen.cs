using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkerCitizen : AIBase
{
    [SerializeField]
    private Transform[] spots;

    private readonly WaitForSeconds DistanceCheckCached = new WaitForSeconds( 0.1f );
    private readonly WaitForSeconds DefaultWaitTimeCached = new WaitForSeconds( 0.1f );

    // private Player player;
    private readonly float disInteraction = 2.0f;
    private float disInteractionSqr;
    private bool isInteraction = false;
    private Player focusedPlayer;

    protected override void Awake()
    {
        base.Awake();

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();

        disInteractionSqr = Mathf.Pow( disInteraction, 2 );

        Random.InitState( ( int )( Time.time * Mathf.PI * 10000.0f ) );
        target = spots[ Random.Range( 1, spots.Length ) ].position;

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }

        StartCoroutine( CheckPlayerDistance() );
    }

    private IEnumerator CheckPlayerDistance()
    {
        while ( true ) 
        {
            yield return DistanceCheckCached;
            
            if ( isInteraction )
            {
                continue;
            }

            foreach ( Player player in ObjectManager.Instance.Players )
            {
                if ( ( transform.position - player.transform.position ).sqrMagnitude <= disInteractionSqr )
                {
                    focusedPlayer = player;
                    ChangeState( AIState.Interaction );
                    isInteraction = true;
                    break;
                }
            }
        }
    }

    protected IEnumerator Interaction()
    {
        animator.SetInteger( AnimatorParameters.AIState, ( int )AIState.Idle );
        while ( true )
        {
            yield return null;

            nav.isStopped = true;
            if ( ( transform.position - focusedPlayer.transform.position ).sqrMagnitude >= disInteractionSqr )
            {
                yield return DefaultWaitTimeCached;
                nav.isStopped = false;
                isInteraction = false;
                ChangeState( AIState.Idle );
            }

            Vector3 dis =  ( focusedPlayer.transform.position - transform.position );
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
