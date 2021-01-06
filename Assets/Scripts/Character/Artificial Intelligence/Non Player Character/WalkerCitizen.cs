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

        Network.Instance.OnLateConnect += OnLateConnect;

        spots = GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>();
        target = spots[ Random.Range( 1, spots.Length ) ].position;

        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }

        nav.speed = 3.0f;
    }

    protected override IEnumerator Idle()
    {
        Debug.Log( "Current State : Idle" );
        while ( true )
        {
            yield return null;

            int index = Random.Range( 1, spots.Length );
            if ( !ReferenceEquals( target, spots[index] ) )
            {
                target = spots[index].position;
                
                yield return new WaitForSeconds( 1.0f );
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
            if ( !nav.pathPending && nav.remainingDistance <= 2.0f ) 
            {
                ChangeState( AIState.Idle );
                nav.isStopped = true;
            }
        }
    }

    protected void OnLateConnect()
    {
        Protocol.ToServer.RequestNpcInfo protocol;
        protocol.NpcInfo.NpcId = gameObject.name;
        protocol.NpcInfo.State = ( int )state;
        protocol.NpcInfo.Target = target;
        protocol.NpcInfo.CurPosition = transform.position;
        Network.Instance.Send( protocol );

        Protocol.Both.SyncNpcState syncNpcState;
        syncNpcState.NpcInfo.NpcId = gameObject.name;
        syncNpcState.NpcInfo.State = ( int )state;
        syncNpcState.NpcInfo.Target = target;
        syncNpcState.NpcInfo.CurPosition = transform.position;
        Network.Instance.Send( syncNpcState );
    }
}
