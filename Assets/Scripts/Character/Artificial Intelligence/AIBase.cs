using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public abstract class AIBase : Character
{
    protected enum AIState : byte
    {
        /* 기본 상태 */
        Idle = 0,

        /* 추가 상태 */
        Attack,
        Patrol,
        Move,
        Dash,
        Dead,
    };

    protected AIState state { get; private set; } = AIState.Idle;
    private Coroutine currentCoroutine = null;

    protected virtual void Start()
    {
        ChangeState( state );
    }

    protected void ChangeState( AIState _state )
    {
        if ( !ReferenceEquals( currentCoroutine, null ) )
        {
            StopCoroutine( currentCoroutine );
        }
        state = _state;
        currentCoroutine = StartCoroutine( _state.ToString() );
    }

    protected abstract IEnumerator Idle();

    protected virtual void OnExit()
    {
        StopAllCoroutines();
    }
}
