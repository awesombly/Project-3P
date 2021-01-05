using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NPCBase : AIBase
{
    protected int spotCount
    {
        get
        {
            return spots.Count;
        }
    }
    private Transform myTransform;
    [SerializeField]
    private List<Transform> spots = new List<Transform>();
    [SerializeField]
    private Transform currentSpot;
    private void Awake()
    {
        myTransform = transform;

        spots.AddRange( GameObject.Find( "RinSpots" ).GetComponentsInChildren<Transform>() );
        spots.RemoveAt( 0 );
        
        if ( ReferenceEquals( spots, null ) )
        {
            Debug.Log( "spots not found" );
        }
    }

    protected override IEnumerator Idle()
    {
        Debug.Log( "Current State : Idle" );
        while( true )
        {
            yield return null;
            
            currentSpot = spots[ Random.Range( 0, spotCount - 1 ) ];
            if ( ReferenceEquals( currentSpot, null ) )
            {
                Debug.Log( "spot setting failed" );
            }

            ChangeState( AIState.Move );
        }
    }

    protected override IEnumerator Move()
    {
        Debug.Log( "Current State : Move" );
        if ( ReferenceEquals( currentSpot, null ) )
        {
            Debug.Log( "State Move Not Found Spots" );
            ChangeState( AIState.Idle );
        }

        while ( true )
        {
            Vector3 dis = ( currentSpot.position - myTransform.position ).normalized;
            dis.y = 0.0f;
            myTransform.Translate( dis * 10.0f * Time.deltaTime );

            yield return null;
            if ( Vector3.Distance( myTransform.position, currentSpot.position ) <= 5.0f ) 
            {
                ChangeState( AIState.Idle );
            }
        }
    }
}
