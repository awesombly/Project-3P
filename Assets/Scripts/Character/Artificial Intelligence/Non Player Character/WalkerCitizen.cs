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

    protected virtual void OnTriggerEnter( Collider _other )
    {
        if ( !isLocal && isInteraction )
        {
            return;
        }

        if ( focusedPlayer == null && _other.CompareTag( "Player" ) )
        {
            focusedPlayer = _other.GetComponent<Actor>() as Player;
            StopAllCoroutines();
            target = _other.transform.position;
            ChangeState( AIState.LookAtTarget );
        }
    }

    protected virtual void OnTriggerStay( Collider _other )
    {
        if ( !isLocal )
        {
            return;
        }

        if ( isInteraction && _other.CompareTag( "Player" ) )
        {
            float Offset = ( target - _other.transform.position ).sqrMagnitude;
            if ( Offset >= 0.05f )
            {
                target = _other.transform.position;

                Protocol.Both.SyncNpcTarget protocol;
                protocol.Serial = serial;
                protocol.Target = target;

                Network.Instance.Send( protocol );
            }
        }
    }

    protected virtual void OnTriggerExit( Collider _other )
    {
        if ( !isLocal && focusedPlayer != null )
        {
            return;
        }

        if ( isInteraction && _other.CompareTag( "Player" ) )
        {
            if ( !focusedPlayer.serial.Equals( _other.GetComponent<Actor>().serial ) )
            {
                return;
            }

            focusedPlayer = null;
            StopAllCoroutines();
            ChangeState( AIState.Idle );
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
                continue;
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
            nav.SetDestination( target );
            if ( !nav.pathPending && nav.remainingDistance <= 2.0f ) 
            {
                nav.isStopped = true;
                if ( isLocal )
                {
                    ChangeState( AIState.Idle );
                }
            }

            yield return null;
        }
    }

    protected virtual IEnumerator LookAtTarget()
    {
        nav.avoidancePriority = 40;
        animator.SetInteger( AnimatorParameters.AIState, ( int )AIState.Idle );
        isInteraction = true;
        nav.isStopped = true;

        while ( true )
        {
            Vector3 dis = target - transform.position;
            Vector3 disXZ = new Vector3( dis.x, 0.0f, dis.z );
            transform.rotation = Quaternion.Lerp( transform.rotation, Quaternion.LookRotation( disXZ ), 0.1f );

            yield return null;
        }
    }

    protected override IEnumerator Interaction()
    {
        yield return null;
    }
}
